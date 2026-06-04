#include "../include/RoutePlanner.hpp"
#include "../include/CityMap.hpp"
#include <unordered_map>
#include <queue>
#include <algorithm>
using namespace std;

// Node used inside Dijkstra's priority queue
struct DNode {
    double cost;
    string id;
    // operator> so priority_queue gives us the SMALLEST cost first
    bool operator>(const DNode& other) const {
        return cost > other.cost;
    }
};

vector<string> RoutePlanner::findRoute(CityMap& map, const string& start, const string& dest) {
    vector<string> noAvoid;
    return rerouteAvoiding(map, start, dest, noAvoid);
}

vector<string> RoutePlanner::rerouteAvoiding(CityMap& map,
                                              const string& current,
                                              const string& dest,
                                              const vector<string>& avoidRoads) {
    const double INF = 1.0e18; // Represents "not reached yet"

    // dist[X] = shortest distance found so far from 'current' to intersection X
    unordered_map<string, double> dist;

    // prev[X] = which intersection we came from to reach X on the shortest path
    unordered_map<string, string> prev;

    // Min-heap: processes the intersection with the smallest cost first
    priority_queue<DNode, vector<DNode>, greater<DNode>> pq;

    // Set all intersections to infinity distance
    unordered_map<string, Intersection>& allIntersections = map.getAllIntersections();
    for (pair<const string, Intersection>& entry : allIntersections) {
        dist[entry.first] = INF;
    }

    // Starting intersection must exist in the map
    if (dist.count(current) == 0) {
        return vector<string>();
    }

    // Distance from start to itself is 0
    dist[current] = 0.0;

    DNode startNode;
    startNode.cost = 0.0;
    startNode.id   = current;
    pq.push(startNode);

    // --- Dijkstra's main loop ---
    while (!pq.empty()) {
        DNode top      = pq.top();
        pq.pop();

        double cost    = top.cost;
        string nodeId  = top.id;

        // Skip outdated entries (we already found a shorter path to this node)
        if (cost > dist[nodeId]) {
            continue;
        }

        // Stop early once we reach the destination
        if (nodeId == dest) {
            break;
        }

        // Explore all roads leaving this intersection
        const vector<string>& outgoing = map.getOutgoingRoads(nodeId);
        for (int i = 0; i < (int)outgoing.size(); i++) {
            string roadId = outgoing[i];

            const RoadSegment* road = map.getRoad(roadId);
            if (road == nullptr || road->blocked) {
                continue;
            }

            // Skip roads we want to avoid (e.g. a blocked road the vehicle just encountered)
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

            // If this new path is shorter, update and re-queue the neighbour
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
        return vector<string>();
    }

    // Rebuild the path by walking backwards from dest to start using 'prev'
    vector<string> path;
    string node = dest;
    while (node != current) {
        path.push_back(node);
        if (prev.count(node) == 0) {
            return vector<string>(); // path is broken
        }
        node = prev[node];
    }
    path.push_back(current);

    // Reverse so the path goes from start -> dest
    reverse(path.begin(), path.end());
    return path;
}
