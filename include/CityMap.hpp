#pragma once
#include "Road.hpp"
#include "Intersection.hpp"
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

class CityMap {
private:
    unordered_map<string, RoadSegment> roads;
    unordered_map<string, Intersection> intersections;
    unordered_map<string, vector<string>> adjacencyList;

public:
    void addRoad(const RoadSegment& road);
    void addIntersection(const string& id);

    RoadSegment* getRoad(const string& id);
    const RoadSegment* getRoad(const string& id) const;
    Intersection* getIntersection(const string& id);

    unordered_map<string, RoadSegment>& getAllRoads() { return roads; }
    const unordered_map<string, RoadSegment>& getAllRoads() const { return roads; }
    unordered_map<string, Intersection>& getAllIntersections() { return intersections; }
    const unordered_map<string, Intersection>& getAllIntersections() const { return intersections; }

    const vector<string>& getOutgoingRoads(const string& intersectionId) const;

    bool roadExists(const string& id) const { return roads.count(id) > 0; }
    bool intersectionExists(const string& id) const { return intersections.count(id) > 0; }

    vector<string> getBlockedRoads() const;
    vector<string> getCongestedRoads() const;
    void incrementVehicleCount(const string& roadId);
    void decrementVehicleCount(const string& roadId);
};
