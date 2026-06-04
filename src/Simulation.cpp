#include "../include/Simulation.hpp"
#include "../include/FileManager.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>

// Comparison function used to sort vehicles: higher priority number goes first
bool compareByPriority(const std::shared_ptr<Vehicle>& a, const std::shared_ptr<Vehicle>& b) {
    return a->getPriority() > b->getPriority();
}

// ---------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------
Simulation::Simulation()
    : currentStep(0), completedCount(0), totalWaitTime(0.0) {}

// ---------------------------------------------------------------
// Add a vehicle to the simulation
// ---------------------------------------------------------------
void Simulation::addVehicle(std::shared_ptr<Vehicle> v) {
    vehicles.push_back(v);
}

// ---------------------------------------------------------------
// Register a traffic light for an intersection
// ---------------------------------------------------------------
void Simulation::addTrafficLight(const std::string& intersectionId, const TrafficLight& tl) {
    trafficLights[intersectionId] = tl;
}

// ---------------------------------------------------------------
// Load all data files
// ---------------------------------------------------------------
void Simulation::loadFromFiles(const std::string& roadsFile,
                                const std::string& vehiclesFile,
                                const std::string& accidentsFile) {
    FileManager::loadRoads(roadsFile, *this);
    FileManager::loadVehicles(vehiclesFile, *this);
    FileManager::loadAccidents(accidentsFile, *this);
}

// ---------------------------------------------------------------
// Plan routes for every vehicle before the simulation starts
// ---------------------------------------------------------------
void Simulation::initialize() {
    for (int i = 0; i < (int)vehicles.size(); i++) {
        std::shared_ptr<Vehicle>& v = vehicles[i];

        const std::vector<std::string>& partial = v->getRoute();
        if ((int)partial.size() < 2) {
            std::cerr << "Vehicle " << v->getId() << " has incomplete route data.\n";
            v->setStatus(VehicleStatus::WAITING);
            continue;
        }

        std::string start = partial[0];
        std::string dest  = partial[1];

        if (!cityMap.intersectionExists(start) || !cityMap.intersectionExists(dest)) {
            std::cerr << "Vehicle " << v->getId() << ": invalid start/dest intersection.\n";
            v->setStatus(VehicleStatus::WAITING);
            continue;
        }

        std::vector<std::string> fullRoute = routePlanner.findRoute(cityMap, start, dest);
        if (fullRoute.empty()) {
            std::cerr << "No route found for vehicle " << v->getId()
                      << " (" << start << " -> " << dest << ").\n";
            v->setStatus(VehicleStatus::WAITING);
            continue;
        }

        v->setRoute(fullRoute);
        v->setStatus(VehicleStatus::MOVING);

        // Place vehicle on the first road of its route
        if ((int)fullRoute.size() >= 2) {
            const std::vector<std::string>& outgoing = cityMap.getOutgoingRoads(fullRoute[0]);
            for (int j = 0; j < (int)outgoing.size(); j++) {
                const RoadSegment* road = cityMap.getRoad(outgoing[j]);
                if (road != nullptr && road->toId == fullRoute[1] && !road->blocked) {
                    v->setCurrentRoad(outgoing[j]);
                    v->setPosition(0.0);
                    cityMap.incrementVehicleCount(outgoing[j]);
                    break;
                }
            }
        }
    }
}

// ---------------------------------------------------------------
// Advance every traffic light by one step
// ---------------------------------------------------------------
void Simulation::updateTrafficLights() {
    for (std::pair<const std::string, TrafficLight>& entry : trafficLights) {
        entry.second.update();
    }
}

// ---------------------------------------------------------------
// Refresh accident effects on roads
// ---------------------------------------------------------------
void Simulation::updateAccidents() {
    accidentManager.update(currentStep, cityMap);
}

// ---------------------------------------------------------------
// Let waiting vehicles through if their intersection is now green
// ---------------------------------------------------------------
void Simulation::releaseWaitingVehicles() {
    std::unordered_map<std::string, Intersection>& allIntersections = cityMap.getAllIntersections();

    for (std::pair<const std::string, Intersection>& entry : allIntersections) {
        std::string interId    = entry.first;
        Intersection& inter    = entry.second;

        // Check whether the traffic light at this intersection is green
        bool green = true; // default: pass through if there is no light
        if (trafficLights.count(interId) > 0) {
            green = trafficLights.at(interId).isGreen();
        }

        if (!green) {
            continue;
        }

        // Release all vehicles waiting in the priority queue at this intersection
        while (inter.hasWaiting()) {
            std::string vId = inter.getNextVehicle();
            for (int i = 0; i < (int)vehicles.size(); i++) {
                if (vehicles[i]->getId() == vId &&
                    vehicles[i]->getStatus() == VehicleStatus::WAITING) {
                    vehicles[i]->setStatus(VehicleStatus::MOVING);
                    break;
                }
            }
        }
    }
}

// ---------------------------------------------------------------
// Move a single vehicle forward by one simulation step
// ---------------------------------------------------------------
void Simulation::processVehicle(std::shared_ptr<Vehicle>& v) {
    // Nothing to do for completed vehicles
    if (v->getStatus() == VehicleStatus::COMPLETED) {
        return;
    }

    // Waiting vehicles just accumulate wait time
    if (v->getStatus() == VehicleStatus::WAITING) {
        v->addWaitTime(1.0);
        totalWaitTime += 1.0;
        return;
    }

    std::string roadId = v->getCurrentRoad();

    // No road means the vehicle just finished its journey
    if (roadId.empty()) {
        v->setStatus(VehicleStatus::COMPLETED);
        completedCount++;
        return;
    }

    RoadSegment* road = cityMap.getRoad(roadId);
    if (road == nullptr) {
        v->setStatus(VehicleStatus::WAITING);
        return;
    }

    // ---- Road is blocked: try to find a detour ----
    if (road->blocked) {
        std::string currentInter = v->getCurrentIntersection();
        std::string dest         = v->getDestination();

        cityMap.decrementVehicleCount(roadId);
        v->setCurrentRoad("");

        if (!currentInter.empty() && currentInter != dest) {
            std::vector<std::string> avoidList;
            avoidList.push_back(roadId);

            std::vector<std::string> newRoute =
                routePlanner.rerouteAvoiding(cityMap, currentInter, dest, avoidList);

            if (!newRoute.empty()) {
                v->setStatus(VehicleStatus::REROUTING);
                v->setRoute(newRoute);

                if ((int)newRoute.size() >= 2) {
                    const std::vector<std::string>& outgoing = cityMap.getOutgoingRoads(newRoute[0]);
                    for (int i = 0; i < (int)outgoing.size(); i++) {
                        RoadSegment* r = cityMap.getRoad(outgoing[i]);
                        if (r != nullptr && !r->blocked && r->toId == newRoute[1]) {
                            v->setCurrentRoad(outgoing[i]);
                            v->setPosition(0.0);
                            cityMap.incrementVehicleCount(outgoing[i]);
                            return;
                        }
                    }
                }
            }
        }

        // No detour available: vehicle must wait
        v->setStatus(VehicleStatus::WAITING);
        v->addWaitTime(1.0);
        totalWaitTime += 1.0;
        return;
    }

    // ---- Move vehicle along the current road ----
    double effectiveSpeed = v->getSpeed();
    if (road->currentSpeedLimit < effectiveSpeed) {
        effectiveSpeed = road->currentSpeedLimit;
    }

    // Convert km/h to m/s, then multiply by 1 second per step
    double distPerStep = effectiveSpeed * (1000.0 / 3600.0);
    double newPos      = v->getPosition() + distPerStep;

    // Vehicle hasn't reached the end of this road yet
    if (newPos < road->length) {
        v->setPosition(newPos);
        v->setStatus(VehicleStatus::MOVING);
        return;
    }

    // ---- Vehicle reached the end intersection of this road ----
    cityMap.decrementVehicleCount(roadId);
    v->advanceRouteIndex();

    const std::vector<std::string>& route = v->getRoute();
    int idx = v->getRouteIndex();

    // Vehicle reached its destination
    if (idx >= (int)route.size() - 1) {
        v->setStatus(VehicleStatus::COMPLETED);
        v->setCurrentRoad("");
        completedCount++;
        return;
    }

    // ---- Check the traffic light at this intersection ----
    std::string atIntersection = route[idx];

    bool green = true; // default: pass through if there is no light
    if (trafficLights.count(atIntersection) > 0) {
        green = trafficLights.at(atIntersection).isGreen();
    }

    if (!green) {
        // Red light: add vehicle to the intersection waiting queue
        Intersection* inter = cityMap.getIntersection(atIntersection);
        if (inter != nullptr) {
            inter->addWaitingVehicle(v->getId(), v->getPriority());
        }
        v->setStatus(VehicleStatus::WAITING);
        v->setCurrentRoad("");
        v->addWaitTime(1.0);
        totalWaitTime += 1.0;
        return;
    }

    // ---- Move vehicle onto the next road ----
    std::string nextInter = route[idx + 1];
    const std::vector<std::string>& outgoing = cityMap.getOutgoingRoads(atIntersection);

    for (int i = 0; i < (int)outgoing.size(); i++) {
        RoadSegment* r = cityMap.getRoad(outgoing[i]);
        if (r != nullptr && !r->blocked && r->toId == nextInter) {
            v->setCurrentRoad(outgoing[i]);
            v->setPosition(0.0);
            v->setStatus(VehicleStatus::MOVING);
            cityMap.incrementVehicleCount(outgoing[i]);
            return;
        }
    }

    // Next road is blocked: try to reroute from current intersection
    std::string dest = v->getDestination();
    std::vector<std::string> newRoute = routePlanner.findRoute(cityMap, atIntersection, dest);

    if (!newRoute.empty()) {
        v->setRoute(newRoute);
        v->setStatus(VehicleStatus::REROUTING);

        if ((int)newRoute.size() >= 2) {
            const std::vector<std::string>& newOut = cityMap.getOutgoingRoads(newRoute[0]);
            for (int i = 0; i < (int)newOut.size(); i++) {
                RoadSegment* r = cityMap.getRoad(newOut[i]);
                if (r != nullptr && !r->blocked && r->toId == newRoute[1]) {
                    v->setCurrentRoad(newOut[i]);
                    v->setPosition(0.0);
                    cityMap.incrementVehicleCount(newOut[i]);
                    return;
                }
            }
        }
    }

    // No route available: vehicle waits
    v->setStatus(VehicleStatus::WAITING);
    v->addWaitTime(1.0);
    totalWaitTime += 1.0;
}

// ---------------------------------------------------------------
// Run one full simulation step
// ---------------------------------------------------------------
void Simulation::runStep() {
    currentStep++;
    updateTrafficLights();
    updateAccidents();
    releaseWaitingVehicles();

    // Higher-priority vehicles (e.g. emergency) move before others
    std::sort(vehicles.begin(), vehicles.end(), compareByPriority);

    for (int i = 0; i < (int)vehicles.size(); i++) {
        processVehicle(vehicles[i]);
    }
}

// ---------------------------------------------------------------
// Run the simulation for a given number of steps
// ---------------------------------------------------------------
void Simulation::run(int steps) {
    for (int i = 0; i < steps; i++) {
        runStep();
        printStepStatus(std::cout);
    }
    saveResults("simulation_result.txt");
}

// ---------------------------------------------------------------
// Print the current state of the simulation
// ---------------------------------------------------------------
void Simulation::printStepStatus(std::ostream& out) const {
    out << "\n--- Simulation Step " << currentStep << " ---\n\n";

    // Traffic lights
    for (const std::pair<const std::string, TrafficLight>& entry : trafficLights) {
        entry.second.printStatus(out);
    }
    out << "\n";

    // Accidents
    accidentManager.printActiveAccidents(out);
    out << "\n";

    // Vehicles still in transit
    for (int i = 0; i < (int)vehicles.size(); i++) {
        if (vehicles[i]->getStatus() == VehicleStatus::COMPLETED) {
            continue;
        }

        vehicles[i]->printStatus(out);

        std::string rid = vehicles[i]->getCurrentRoad();
        if (!rid.empty()) {
            const RoadSegment* road = cityMap.getRoad(rid);
            if (road != nullptr) {
                out << "  Road Length: " << road->length << " m\n";
                out << "  Position: " << std::fixed << std::setprecision(1)
                    << vehicles[i]->getPosition() << "/" << road->length << " m\n";
            }
        }

        if (vehicles[i]->getStatus() == VehicleStatus::REROUTING) {
            out << "  Status: Re-routing because road is blocked\n";
        }
        out << "\n";
    }

    // Summary statistics
    int total = (int)vehicles.size();
    double avgWait = 0.0;
    if (total > 0) {
        avgWait = totalWaitTime / total;
    }

    out << "Completed Vehicles: " << completedCount << "\n";
    out << "Average Waiting Time: " << std::fixed << std::setprecision(1)
        << avgWait << " seconds\n";

    std::vector<std::string> blocked = cityMap.getBlockedRoads();
    out << "Blocked Roads: ";
    if (blocked.empty()) {
        out << "None";
    } else {
        for (int i = 0; i < (int)blocked.size(); i++) {
            if (i > 0) out << ", ";
            out << blocked[i];
        }
    }
    out << "\n";

    std::vector<std::string> congested = cityMap.getCongestedRoads();
    out << "Congested Roads: ";
    if (congested.empty()) {
        out << "None";
    } else {
        for (int i = 0; i < (int)congested.size(); i++) {
            if (i > 0) out << ", ";
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
