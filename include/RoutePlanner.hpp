#pragma once
#include <string>
#include <vector>

class CityMap;

class RoutePlanner {
public:
    std::vector<std::string> findRoute(CityMap& map,
                                       const std::string& start,
                                       const std::string& dest);

    std::vector<std::string> rerouteAvoiding(CityMap& map,
                                              const std::string& current,
                                              const std::string& dest,
                                              const std::vector<std::string>& avoidRoads);
};
