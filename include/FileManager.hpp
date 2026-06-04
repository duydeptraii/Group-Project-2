#pragma once
#include <string>
using namespace std;

class Simulation;

class FileManager {
public:
    static void loadRoads(const string& filename, Simulation& sim);
    static void loadVehicles(const string& filename, Simulation& sim);
    static void loadAccidents(const string& filename, Simulation& sim);
    static void saveResults(const string& filename, const Simulation& sim);
};
