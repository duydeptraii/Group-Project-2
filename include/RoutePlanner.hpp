#pragma once
#include <string>
#include <vector>
using namespace std;

class CityMap;

class RoutePlanner {
public:
    vector<string> findRoute(CityMap& map, const string& start, const string& dest);

    vector<string> rerouteAvoiding(CityMap& map, const string& current,
                                   const string& dest,
                                   const vector<string>& avoidRoads);
};
