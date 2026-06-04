#include "../include/CityMap.hpp"
using namespace std;

// Returned when an intersection has no outgoing roads
static const vector<string> EMPTY_VEC;

void CityMap::addRoad(const RoadSegment& road) {
    roads[road.id] = road;

    if (intersections.count(road.fromId) == 0) {
        intersections[road.fromId] = Intersection(road.fromId);
    }
    if (intersections.count(road.toId) == 0) {
        intersections[road.toId] = Intersection(road.toId);
    }

    adjacencyList[road.fromId].push_back(road.id);
}

void CityMap::addIntersection(const string& id) {
    if (intersections.count(id) == 0) {
        intersections[id] = Intersection(id);
    }
}

RoadSegment* CityMap::getRoad(const string& id) {
    if (roads.count(id) == 0) {
        return nullptr;
    }
    return &roads[id];
}

const RoadSegment* CityMap::getRoad(const string& id) const {
    if (roads.count(id) == 0) {
        return nullptr;
    }
    return &roads.at(id);
}

Intersection* CityMap::getIntersection(const string& id) {
    if (intersections.count(id) == 0) {
        return nullptr;
    }
    return &intersections[id];
}

const vector<string>& CityMap::getOutgoingRoads(const string& intersectionId) const {
    if (adjacencyList.count(intersectionId) == 0) {
        return EMPTY_VEC;
    }
    return adjacencyList.at(intersectionId);
}

vector<string> CityMap::getBlockedRoads() const {
    vector<string> result;
    for (const pair<const string, RoadSegment>& entry : roads) {
        if (entry.second.blocked) {
            result.push_back(entry.first);
        }
    }
    return result;
}

vector<string> CityMap::getCongestedRoads() const {
    vector<string> result;
    for (const pair<const string, RoadSegment>& entry : roads) {
        if (entry.second.isCongested()) {
            result.push_back(entry.first);
        }
    }
    return result;
}

void CityMap::incrementVehicleCount(const string& roadId) {
    if (roads.count(roadId) > 0) {
        roads[roadId].vehicleCount++;
    }
}

void CityMap::decrementVehicleCount(const string& roadId) {
    if (roads.count(roadId) > 0 && roads[roadId].vehicleCount > 0) {
        roads[roadId].vehicleCount--;
    }
}
