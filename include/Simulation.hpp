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

class Simulation {
private:
    CityMap cityMap;
    RoutePlanner routePlanner;
    AccidentManager accidentManager;
    std::unordered_map<std::string, TrafficLight> trafficLights;
    std::vector<std::shared_ptr<Vehicle>> vehicles;
    int currentStep;
    int completedCount;
    double totalWaitTime;

    void updateTrafficLights();
    void updateAccidents();
    void processVehicle(std::shared_ptr<Vehicle>& vehicle);
    void releaseWaitingVehicles();
    void printStepStatus(std::ostream& out) const;

public:
    Simulation();

    void loadFromFiles(const std::string& roadsFile,
                       const std::string& vehiclesFile,
                       const std::string& accidentsFile);

    void initialize();
    void runStep();
    void run(int steps);
    void printStatus(std::ostream& out) const;
    void saveResults(const std::string& outputFile) const;

    CityMap& getCityMap() { return cityMap; }
    const CityMap& getCityMap() const { return cityMap; }
    AccidentManager& getAccidentManager() { return accidentManager; }
    const AccidentManager& getAccidentManager() const { return accidentManager; }
    const std::unordered_map<std::string, TrafficLight>& getTrafficLights() const { return trafficLights; }
    const std::vector<std::shared_ptr<Vehicle>>& getVehicles() const { return vehicles; }
    int getCurrentStep() const { return currentStep; }
    int getCompletedCount() const { return completedCount; }
    double getTotalWaitTime() const { return totalWaitTime; }

    void addVehicle(std::shared_ptr<Vehicle> v);
    void addTrafficLight(const std::string& intersectionId, const TrafficLight& tl);
};
