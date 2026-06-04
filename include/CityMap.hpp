#pragma once
#include "Road.hpp"
#include "Intersection.hpp"
#include <unordered_map>
#include <vector>
#include <string>

class CityMap {
private:
    std::unordered_map<std::string, RoadSegment> roads;
    std::unordered_map<std::string, Intersection> intersections;
    std::unordered_map<std::string, std::vector<std::string>> adjacencyList;

public:
    void addRoad(const RoadSegment& road);
    void addIntersection(const std::string& id);

    RoadSegment* getRoad(const std::string& id);
    const RoadSegment* getRoad(const std::string& id) const;
    Intersection* getIntersection(const std::string& id);

    std::unordered_map<std::string, RoadSegment>& getAllRoads() { return roads; }
    const std::unordered_map<std::string, RoadSegment>& getAllRoads() const { return roads; }
    std::unordered_map<std::string, Intersection>& getAllIntersections() { return intersections; }
    const std::unordered_map<std::string, Intersection>& getAllIntersections() const { return intersections; }

    const std::vector<std::string>& getOutgoingRoads(const std::string& intersectionId) const;

    bool roadExists(const std::string& id) const { return roads.count(id) > 0; }
    bool intersectionExists(const std::string& id) const { return intersections.count(id) > 0; }

    std::vector<std::string> getBlockedRoads() const;
    std::vector<std::string> getCongestedRoads() const;
    void incrementVehicleCount(const std::string& roadId);
    void decrementVehicleCount(const std::string& roadId);
};
