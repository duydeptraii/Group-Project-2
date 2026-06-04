#pragma once
#include "TrafficAccident.hpp"
#include <vector>
#include <memory>
#include <string>
#include <ostream>
using namespace std;

class CityMap;

class AccidentManager {
private:
    vector<unique_ptr<TrafficAccident>> accidents;

public:
    void addAccident(unique_ptr<TrafficAccident> acc);
    void update(int currentTime, CityMap& map);

    const vector<unique_ptr<TrafficAccident>>& getAccidents() const { return accidents; }
    vector<string> getBlockedRoads() const;
    void printActiveAccidents(ostream& out) const;
};
