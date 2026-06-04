#include "../include/CityMap.hpp"

// Returned when an intersection has no outgoing roads
static const std::vector<std::string> EMPTY_VEC;

void CityMap::addRoad(const RoadSegment& road) {
    roads[road.id] = road;

    // Add both endpoint intersections if they don't exist yet
    if (intersections.count(road.fromId) == 0) {
        intersections[road.fromId] = Intersection(road.fromId);
    }
    if (intersections.count(road.toId) == 0) {
        intersections[road.toId] = Intersection(road.toId);
    }

    adjacencyList[road.fromId].push_back(road.id);
}

void CityMap::addIntersection(const std::string& id) {
    if (intersections.count(id) == 0) {
        intersections[id] = Intersection(id);
    }
}

RoadSegment* CityMap::getRoad(const std::string& id) {
    if (roads.count(id) == 0) {
        return nullptr;
    }
    return &roads[id];
}

const RoadSegment* CityMap::getRoad(const std::string& id) const {
    if (roads.count(id) == 0) {
        return nullptr;
    }
    return &roads.at(id);
}

Intersection* CityMap::getIntersection(const std::string& id) {
    if (intersections.count(id) == 0) {
        return nullptr;
    }
    return &intersections[id];
}

const std::vector<std::string>& CityMap::getOutgoingRoads(const std::string& intersectionId) const {
    if (adjacencyList.count(intersectionId) == 0) {
        return EMPTY_VEC;
    }
    return adjacencyList.at(intersectionId);
}

std::vector<std::string> CityMap::getBlockedRoads() const {
    std::vector<std::string> result;
    for (const std::pair<const std::string, RoadSegment>& entry : roads) {
        if (entry.second.blocked) {
            result.push_back(entry.first);
        }
    }
    return result;
}

std::vector<std::string> CityMap::getCongestedRoads() const {
    std::vector<std::string> result;
    for (const std::pair<const std::string, RoadSegment>& entry : roads) {
        if (entry.second.isCongested()) {
            result.push_back(entry.first);
        }
    }
    return result;
}

void CityMap::incrementVehicleCount(const std::string& roadId) {
    if (roads.count(roadId) > 0) {
        roads[roadId].vehicleCount++;
    }
}

void CityMap::decrementVehicleCount(const std::string& roadId) {
    if (roads.count(roadId) > 0 && roads[roadId].vehicleCount > 0) {
        roads[roadId].vehicleCount--;
    }
}
