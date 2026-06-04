#include "../include/AccidentManager.hpp"
#include "../include/CityMap.hpp"
#include <iostream>

void AccidentManager::addAccident(std::unique_ptr<TrafficAccident> acc) {
    accidents.push_back(std::move(acc));
}

void AccidentManager::update(int currentTime, CityMap& map) {
    for (auto& acc : accidents) {
        bool wasActive = acc->isActive();

        acc->update(currentTime);

        RoadSegment* road = map.getRoad(acc->getRoadId());
        if (!road) continue;

        if (acc->isActive()) {
            if (acc->doesBlockRoad()) {
                road->blocked = true;
                road->currentSpeedLimit = 0.0;
            } else {
                road->blocked = false;
                road->reduceSpeed(acc->getSpeedFactor());
            }
        } else if (wasActive) {
            road->blocked = false;
            road->restoreSpeed();
        }
    }
}

std::vector<std::string> AccidentManager::getBlockedRoads() const {
    std::vector<std::string> blocked;
    for (const auto& acc : accidents) {
        if (acc->doesBlockRoad()) {
            blocked.push_back(acc->getRoadId());
        }
    }
    return blocked;
}

void AccidentManager::printActiveAccidents(std::ostream& out) const {
    bool hasActive = false;
    for (const auto& acc : accidents) {
        if (acc->isActive()) {
            if (!hasActive) {
                out << "Active Accidents:\n";
                hasActive = true;
            }
            acc->printStatus(out);
        }
    }
    if (!hasActive) {
        out << "Active Accidents: None\n";
    }
}
