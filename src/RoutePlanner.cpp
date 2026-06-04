#include "../include/RoutePlanner.hpp"
#include "../include/CityMap.hpp"
#include <unordered_map>
#include <queue>
#include <algorithm>

// Node used inside Dijkstra's priority queue
struct DNode {
    double cost;
    std::string id;
    // operator> so priority_queue gives us the SMALLEST cost first
    bool operator>(const DNode& other) const {
        return cost > other.cost;
    }
};

std::vector<std::string> RoutePlanner::findRoute(CityMap& map,
                                                  const std::string& start,
                                                  const std::string& dest) {
    std::vector<std::string> noAvoid;
    return rerouteAvoiding(map, start, dest, noAvoid);
}

std::vector<std::string> RoutePlanner::rerouteAvoiding(CityMap& map,
                                                        const std::string& current,
                                                        const std::string& dest,
                                                        const std::vector<std::string>& avoidRoads) {
    const double INF = 1.0e18; // Represents "not reached yet"

    // dist[X] = shortest distance found so far from 'current' to intersection X
    std::unordered_map<std::string, double> dist;

    // prev[X] = which intersection we came from to reach X on the shortest path
    std::unordered_map<std::string, std::string> prev;

    // Min-heap: processes intersection with smallest cost first
    std::priority_queue<DNode, std::vector<DNode>, std::greater<DNode>> pq;

    // Set all intersections to infinity distance
    std::unordered_map<std::string, Intersection>& allIntersections = map.getAllIntersections();
    for (std::pair<const std::string, Intersection>& entry : allIntersections) {
        dist[entry.first] = INF;
    }

    // Starting intersection must exist in the map
    if (dist.count(current) == 0) {
        return std::vector<std::string>();
    }

    // Distance from start to itself is 0
    dist[current] = 0.0;

    DNode startNode;
    startNode.cost = 0.0;
    startNode.id   = current;
    pq.push(startNode);

    // --- Dijkstra's main loop ---
    while (!pq.empty()) {
        DNode top = pq.top();
        pq.pop();

        double cost        = top.cost;
        std::string nodeId = top.id;

        // Skip outdated entries (we already found a shorter path)
        if (cost > dist[nodeId]) {
            continue;
        }

        // Stop early once we reach the destination
        if (nodeId == dest) {
            break;
        }

        // Explore all roads leaving this intersection
        const std::vector<std::string>& outgoing = map.getOutgoingRoads(nodeId);
        for (int i = 0; i < (int)outgoing.size(); i++) {
            std::string roadId = outgoing[i];

            const RoadSegment* road = map.getRoad(roadId);
            if (road == nullptr || road->blocked) {
                continue;
            }

            // Skip roads we want to avoid (e.g. because they are blocked)
            bool avoided = false;
            for (int j = 0; j < (int)avoidRoads.size(); j++) {
                if (roadId == avoidRoads[j]) {
                    avoided = true;
                    break;
                }
            }
            if (avoided) {
                continue;
            }

            double newCost = cost + road->length;

            // If this route is shorter, update and re-queue the neighbour
            if (dist.count(road->toId) > 0 && newCost < dist[road->toId]) {
                dist[road->toId] = newCost;
                prev[road->toId] = nodeId;

                DNode nextNode;
                nextNode.cost = newCost;
                nextNode.id   = road->toId;
                pq.push(nextNode);
            }
        }
    }

    // No path found
    if (dist[dest] == INF) {
        return std::vector<std::string>();
    }

    // Rebuild the path by walking backwards from dest to start using 'prev'
    std::vector<std::string> path;
    std::string node = dest;
    while (node != current) {
        path.push_back(node);
        if (prev.count(node) == 0) {
            return std::vector<std::string>(); // path is broken
        }
        node = prev[node];
    }
    path.push_back(current);

    // Reverse so the path goes from start -> dest
    std::reverse(path.begin(), path.end());
    return path;
}
