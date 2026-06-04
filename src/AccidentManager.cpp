#include "../include/AccidentManager.hpp"
#include "../include/CityMap.hpp"
#include <iostream>
using namespace std;

void AccidentManager::addAccident(unique_ptr<TrafficAccident> acc) {
    // unique_ptr cannot be copied, so std::move transfers ownership into the vector
    accidents.push_back(move(acc));
}

void AccidentManager::update(int currentTime, CityMap& map) {
    for (int i = 0; i < (int)accidents.size(); i++) {
        bool wasActive = accidents[i]->isActive();

        accidents[i]->update(currentTime);

        RoadSegment* road = map.getRoad(accidents[i]->getRoadId());
        if (road == nullptr) {
            continue;
        }

        if (accidents[i]->isActive()) {
            if (accidents[i]->doesBlockRoad()) {
                road->blocked = true;
                road->currentSpeedLimit = 0.0;
            } else {
                road->blocked = false;
                road->reduceSpeed(accidents[i]->getSpeedFactor());
            }
        } else if (wasActive) {
            // Accident just ended: restore road to normal
            road->blocked = false;
            road->restoreSpeed();
        }
    }
}

vector<string> AccidentManager::getBlockedRoads() const {
    vector<string> blocked;
    for (int i = 0; i < (int)accidents.size(); i++) {
        if (accidents[i]->doesBlockRoad()) {
            blocked.push_back(accidents[i]->getRoadId());
        }
    }
    return blocked;
}

void AccidentManager::printActiveAccidents(ostream& out) const {
    bool hasActive = false;
    for (int i = 0; i < (int)accidents.size(); i++) {
        if (accidents[i]->isActive()) {
            if (!hasActive) {
                out << "Active Accidents:\n";
                hasActive = true;
            }
            accidents[i]->printStatus(out);
        }
    }
    if (!hasActive) {
        out << "Active Accidents: None\n";
    }
}
