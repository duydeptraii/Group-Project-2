#include "../include/RoutePlanner.hpp"
#include "../include/CityMap.hpp"
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>

struct DNode {
    double cost;
    std::string id;
    bool operator>(const DNode& o) const { return cost > o.cost; }
};

std::vector<std::string> RoutePlanner::findRoute(CityMap& map,
                                                  const std::string& start,
                                                  const std::string& dest) {
    return rerouteAvoiding(map, start, dest, {});
}

std::vector<std::string> RoutePlanner::rerouteAvoiding(CityMap& map,
                                                        const std::string& current,
                                                        const std::string& dest,
                                                        const std::vector<std::string>& avoidRoads) {
    const double INF = std::numeric_limits<double>::infinity();
    std::unordered_map<std::string, double> dist;
    std::unordered_map<std::string, std::string> prev;
    std::priority_queue<DNode, std::vector<DNode>, std::greater<DNode>> pq;

    for (const auto& pair : map.getAllIntersections())
        dist[pair.first] = INF;

    if (dist.find(current) == dist.end()) return {};
    dist[current] = 0.0;
    pq.push({0.0, current});

    while (!pq.empty()) {
        auto [cost, u] = pq.top(); pq.pop();
        if (cost > dist[u]) continue;
        if (u == dest) break;

        for (const std::string& roadId : map.getOutgoingRoads(u)) {
            const RoadSegment* road = map.getRoad(roadId);
            if (!road || road->blocked) continue;

            bool avoided = false;
            for (const auto& aid : avoidRoads)
                if (roadId == aid) { avoided = true; break; }
            if (avoided) continue;

            double newCost = cost + road->length;
            auto it = dist.find(road->toId);
            if (it != dist.end() && newCost < it->second) {
                it->second = newCost;
                prev[road->toId] = u;
                pq.push({newCost, road->toId});
            }
        }
    }

    if (dist[dest] == INF) return {};

    std::vector<std::string> path;
    std::string node = dest;
    while (node != current) {
        path.push_back(node);
        auto it = prev.find(node);
        if (it == prev.end()) return {};
        node = it->second;
    }
    path.push_back(current);
    std::reverse(path.begin(), path.end());
    return path;
}
