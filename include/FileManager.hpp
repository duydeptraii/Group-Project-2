#pragma once
#include <string>

class Simulation;

class FileManager {
public:
    static void loadRoads(const std::string& filename, Simulation& sim);
    static void loadVehicles(const std::string& filename, Simulation& sim);
    static void loadAccidents(const std::string& filename, Simulation& sim);
    static void saveResults(const std::string& filename, const Simulation& sim);
};
