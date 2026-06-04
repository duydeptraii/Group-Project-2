#pragma once
#include "TrafficAccident.hpp"
#include <vector>
#include <memory>
#include <string>
#include <ostream>

class CityMap;

class AccidentManager {
private:
    std::vector<std::unique_ptr<TrafficAccident>> accidents;

public:
    void addAccident(std::unique_ptr<TrafficAccident> acc);
    void update(int currentTime, CityMap& map);

    const std::vector<std::unique_ptr<TrafficAccident>>& getAccidents() const { return accidents; }
    std::vector<std::string> getBlockedRoads() const;
    void printActiveAccidents(std::ostream& out) const;
};
