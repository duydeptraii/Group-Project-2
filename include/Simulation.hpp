#pragma once
#include "CityMap.hpp"
#include "RoutePlanner.hpp"
#include "AccidentManager.hpp"
#include "TrafficLight.hpp"
#include "Vehicle.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <ostream>
using namespace std;

class Simulation {
private:
    CityMap cityMap;
    RoutePlanner routePlanner;
    AccidentManager accidentManager;
    unordered_map<string, TrafficLight> trafficLights;
    vector<shared_ptr<Vehicle>> vehicles;
    int currentStep;
    int completedCount;
    double totalWaitTime;

    void updateTrafficLights();
    void updateAccidents();
    void processVehicle(shared_ptr<Vehicle>& vehicle);
    void releaseWaitingVehicles();
    void printStepStatus(ostream& out) const;

public:
    Simulation();

    void loadFromFiles(const string& roadsFile,
                       const string& vehiclesFile,
                       const string& accidentsFile);

    void initialize();
    void runStep();
    void run(int steps);
    void printStatus(ostream& out) const;
    void saveResults(const string& outputFile) const;

    CityMap& getCityMap() { return cityMap; }
    const CityMap& getCityMap() const { return cityMap; }
    AccidentManager& getAccidentManager() { return accidentManager; }
    const AccidentManager& getAccidentManager() const { return accidentManager; }
    const unordered_map<string, TrafficLight>& getTrafficLights() const { return trafficLights; }
    const vector<shared_ptr<Vehicle>>& getVehicles() const { return vehicles; }
    int getCurrentStep() const { return currentStep; }
    int getCompletedCount() const { return completedCount; }
    double getTotalWaitTime() const { return totalWaitTime; }

    void addVehicle(shared_ptr<Vehicle> v);
    void addTrafficLight(const string& intersectionId, const TrafficLight& tl);
};
