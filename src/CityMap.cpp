#include "../include/CityMap.hpp"

static const std::vector<std::string> EMPTY_VEC;

void CityMap::addRoad(const RoadSegment& road) {
    roads[road.id] = road;
    if (intersections.find(road.fromId) == intersections.end())
        intersections.emplace(road.fromId, Intersection(road.fromId));
    if (intersections.find(road.toId) == intersections.end())
        intersections.emplace(road.toId, Intersection(road.toId));
    adjacencyList[road.fromId].push_back(road.id);
}

void CityMap::addIntersection(const std::string& id) {
    if (intersections.find(id) == intersections.end())
        intersections.emplace(id, Intersection(id));
}

RoadSegment* CityMap::getRoad(const std::string& id) {
    auto it = roads.find(id);
    return (it != roads.end()) ? &it->second : nullptr;
}

const RoadSegment* CityMap::getRoad(const std::string& id) const {
    auto it = roads.find(id);
    return (it != roads.end()) ? &it->second : nullptr;
}

Intersection* CityMap::getIntersection(const std::string& id) {
    auto it = intersections.find(id);
    return (it != intersections.end()) ? &it->second : nullptr;
}

const std::vector<std::string>& CityMap::getOutgoingRoads(const std::string& intersectionId) const {
    auto it = adjacencyList.find(intersectionId);
    return (it != adjacencyList.end()) ? it->second : EMPTY_VEC;
}

std::vector<std::string> CityMap::getBlockedRoads() const {
    std::vector<std::string> result;
    for (const auto& pair : roads) {
        if (pair.second.blocked) result.push_back(pair.first);
    }
    return result;
}

std::vector<std::string> CityMap::getCongestedRoads() const {
    std::vector<std::string> result;
    for (const auto& pair : roads) {
        if (pair.second.isCongested()) result.push_back(pair.first);
    }
    return result;
}

void CityMap::incrementVehicleCount(const std::string& roadId) {
    auto it = roads.find(roadId);
    if (it != roads.end()) ++it->second.vehicleCount;
}

void CityMap::decrementVehicleCount(const std::string& roadId) {
    auto it = roads.find(roadId);
    if (it != roads.end() && it->second.vehicleCount > 0) --it->second.vehicleCount;
}
