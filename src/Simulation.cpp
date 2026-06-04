#include "../include/Simulation.hpp"
#include "../include/FileManager.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>

Simulation::Simulation()
    : currentStep(0), completedCount(0), totalWaitTime(0.0) {}

void Simulation::addVehicle(std::shared_ptr<Vehicle> v) {
    vehicles.push_back(std::move(v));
}

void Simulation::addTrafficLight(const std::string& intersectionId, const TrafficLight& tl) {
    trafficLights.emplace(intersectionId, tl);
}

void Simulation::loadFromFiles(const std::string& roadsFile,
                                const std::string& vehiclesFile,
                                const std::string& accidentsFile) {
    FileManager::loadRoads(roadsFile, *this);
    FileManager::loadVehicles(vehiclesFile, *this);
    FileManager::loadAccidents(accidentsFile, *this);
}

void Simulation::initialize() {
    for (auto& v : vehicles) {
        const auto& partial = v->getRoute();
        if (partial.size() < 2) {
            std::cerr << "Vehicle " << v->getId() << " has incomplete route data.\n";
            v->setStatus(VehicleStatus::WAITING);
            continue;
        }

        std::string start = partial[0];
        std::string dest  = partial[1];

        if (!cityMap.intersectionExists(start) || !cityMap.intersectionExists(dest)) {
            std::cerr << "Vehicle " << v->getId()
                      << ": invalid start/dest intersection.\n";
            v->setStatus(VehicleStatus::WAITING);
            continue;
        }

        auto fullRoute = routePlanner.findRoute(cityMap, start, dest);
        if (fullRoute.empty()) {
            std::cerr << "No route found for vehicle " << v->getId()
                      << " (" << start << " -> " << dest << ").\n";
            v->setStatus(VehicleStatus::WAITING);
            continue;
        }

        v->setRoute(fullRoute);
        v->setStatus(VehicleStatus::MOVING);

        if (fullRoute.size() >= 2) {
            for (const auto& roadId : cityMap.getOutgoingRoads(fullRoute[0])) {
                const RoadSegment* road = cityMap.getRoad(roadId);
                if (road && road->toId == fullRoute[1] && !road->blocked) {
                    v->setCurrentRoad(roadId);
                    v->setPosition(0.0);
                    cityMap.incrementVehicleCount(roadId);
                    break;
                }
            }
        }
    }
}

void Simulation::updateTrafficLights() {
    for (auto& pair : trafficLights)
        pair.second.update();
}

void Simulation::updateAccidents() {
    accidentManager.update(currentStep, cityMap);
}

void Simulation::releaseWaitingVehicles() {
    for (auto& pair : cityMap.getAllIntersections()) {
        Intersection& inter = pair.second;
        auto tlIt = trafficLights.find(pair.first);
        bool green = (tlIt != trafficLights.end()) ? tlIt->second.isGreen() : true;

        if (!green) continue;

        while (inter.hasWaiting()) {
            std::string vId = inter.getNextVehicle();
            for (auto& v : vehicles) {
                if (v->getId() == vId && v->getStatus() == VehicleStatus::WAITING) {
                    v->setStatus(VehicleStatus::MOVING);
                    break;
                }
            }
        }
    }
}

void Simulation::processVehicle(std::shared_ptr<Vehicle>& v) {
    if (v->getStatus() == VehicleStatus::COMPLETED) return;

    if (v->getStatus() == VehicleStatus::WAITING) {
        v->addWaitTime(1.0);
        totalWaitTime += 1.0;
        return;
    }

    const std::string& roadId = v->getCurrentRoad();
    if (roadId.empty()) {
        v->setStatus(VehicleStatus::COMPLETED);
        ++completedCount;
        return;
    }

    RoadSegment* road = cityMap.getRoad(roadId);
    if (!road) {
        v->setStatus(VehicleStatus::WAITING);
        return;
    }

    if (road->blocked) {
        std::string currentInter = v->getCurrentIntersection();
        std::string dest = v->getDestination();

        cityMap.decrementVehicleCount(roadId);
        v->setCurrentRoad("");

        if (!currentInter.empty() && currentInter != dest) {
            auto newRoute = routePlanner.rerouteAvoiding(cityMap, currentInter, dest, {roadId});
            if (!newRoute.empty()) {
                v->setStatus(VehicleStatus::REROUTING);
                v->setRoute(newRoute);
                if (newRoute.size() >= 2) {
                    for (const auto& rid : cityMap.getOutgoingRoads(newRoute[0])) {
                        RoadSegment* r = cityMap.getRoad(rid);
                        if (r && !r->blocked && r->toId == newRoute[1]) {
                            v->setCurrentRoad(rid);
                            v->setPosition(0.0);
                            cityMap.incrementVehicleCount(rid);
                            return;
                        }
                    }
                }
            }
        }
        v->setStatus(VehicleStatus::WAITING);
        v->addWaitTime(1.0);
        totalWaitTime += 1.0;
        return;
    }

    double effectiveSpeed = std::min(v->getSpeed(), road->currentSpeedLimit);
    double distPerStep = effectiveSpeed * (1000.0 / 3600.0);
    double newPos = v->getPosition() + distPerStep;

    if (newPos < road->length) {
        v->setPosition(newPos);
        v->setStatus(VehicleStatus::MOVING);
        return;
    }

    cityMap.decrementVehicleCount(roadId);
    v->advanceRouteIndex();

    const auto& route = v->getRoute();
    int idx = v->getRouteIndex();

    if (idx >= static_cast<int>(route.size()) - 1) {
        v->setStatus(VehicleStatus::COMPLETED);
        v->setCurrentRoad("");
        ++completedCount;
        return;
    }

    std::string atIntersection = route[idx];
    auto tlIt = trafficLights.find(atIntersection);
    bool green = (tlIt == trafficLights.end()) || tlIt->second.isGreen();

    if (!green) {
        Intersection* inter = cityMap.getIntersection(atIntersection);
        if (inter) inter->addWaitingVehicle(v->getId(), v->getPriority());
        v->setStatus(VehicleStatus::WAITING);
        v->setCurrentRoad("");
        v->addWaitTime(1.0);
        totalWaitTime += 1.0;
        return;
    }

    std::string nextInter = route[idx + 1];
    for (const auto& rid : cityMap.getOutgoingRoads(atIntersection)) {
        RoadSegment* r = cityMap.getRoad(rid);
        if (r && !r->blocked && r->toId == nextInter) {
            v->setCurrentRoad(rid);
            v->setPosition(0.0);
            v->setStatus(VehicleStatus::MOVING);
            cityMap.incrementVehicleCount(rid);
            return;
        }
    }

    std::string dest = v->getDestination();
    auto newRoute = routePlanner.findRoute(cityMap, atIntersection, dest);
    if (!newRoute.empty()) {
        v->setRoute(newRoute);
        v->setStatus(VehicleStatus::REROUTING);
        if (newRoute.size() >= 2) {
            for (const auto& rid : cityMap.getOutgoingRoads(newRoute[0])) {
                RoadSegment* r = cityMap.getRoad(rid);
                if (r && !r->blocked && r->toId == newRoute[1]) {
                    v->setCurrentRoad(rid);
                    v->setPosition(0.0);
                    cityMap.incrementVehicleCount(rid);
                    return;
                }
            }
        }
    }
    v->setStatus(VehicleStatus::WAITING);
    v->addWaitTime(1.0);
    totalWaitTime += 1.0;
}

void Simulation::runStep() {
    ++currentStep;
    updateTrafficLights();
    updateAccidents();
    releaseWaitingVehicles();

    std::stable_sort(vehicles.begin(), vehicles.end(),
        [](const std::shared_ptr<Vehicle>& a, const std::shared_ptr<Vehicle>& b) {
            return a->getPriority() > b->getPriority();
        });

    for (auto& v : vehicles)
        processVehicle(v);
}

void Simulation::run(int steps) {
    for (int i = 0; i < steps; ++i) {
        runStep();
        printStepStatus(std::cout);
    }
    saveResults("simulation_result.txt");
}

void Simulation::printStepStatus(std::ostream& out) const {
    out << "\n--- Simulation Step " << currentStep << " ---\n\n";

    for (const auto& pair : trafficLights)
        pair.second.printStatus(out);
    out << "\n";

    accidentManager.printActiveAccidents(out);
    out << "\n";

    for (const auto& v : vehicles) {
        if (v->getStatus() == VehicleStatus::COMPLETED) continue;
        v->printStatus(out);
        const std::string& rid = v->getCurrentRoad();
        if (!rid.empty()) {
            const RoadSegment* road = cityMap.getRoad(rid);
            if (road) {
                out << "  Road Length: " << road->length << " m\n";
                out << "  Position: " << std::fixed << std::setprecision(1)
                    << v->getPosition() << "/" << road->length << " m\n";
            }
        }
        if (v->getStatus() == VehicleStatus::REROUTING)
            out << "  Status: Re-routing because road is blocked\n";
        out << "\n";
    }

    int total = static_cast<int>(vehicles.size());
    double avgWait = (total > 0) ? (totalWaitTime / total) : 0.0;

    out << "Completed Vehicles: " << completedCount << "\n";
    out << "Average Waiting Time: " << std::fixed << std::setprecision(1)
        << avgWait << " seconds\n";

    auto blocked = cityMap.getBlockedRoads();
    out << "Blocked Roads: ";
    if (blocked.empty()) { out << "None"; }
    else {
        for (size_t i = 0; i < blocked.size(); ++i) {
            if (i) out << ", ";
            out << blocked[i];
        }
    }
    out << "\n";

    auto congested = cityMap.getCongestedRoads();
    out << "Congested Roads: ";
    if (congested.empty()) { out << "None"; }
    else {
        for (size_t i = 0; i < congested.size(); ++i) {
            if (i) out << ", ";
            out << congested[i];
        }
    }
    out << "\n";
}

void Simulation::printStatus(std::ostream& out) const {
    printStepStatus(out);
}

void Simulation::saveResults(const std::string& outputFile) const {
    FileManager::saveResults(outputFile, *this);
}
