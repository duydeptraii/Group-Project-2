#include "../include/FileManager.hpp"
#include "../include/Simulation.hpp"
#include "../include/Vehicle.hpp"
#include "../include/Road.hpp"
#include "../include/TrafficAccident.hpp"
#include "../include/TrafficLight.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <memory>

void FileManager::loadRoads(const std::string& filename, Simulation& sim) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filename);

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string id, from, to;
        double length, speedLimit;
        int blocked;
        if (!(iss >> id >> from >> to >> length >> speedLimit >> blocked)) {
            std::cerr << "Warning: malformed road line: " << line << "\n";
            continue;
        }
        sim.getCityMap().addRoad(RoadSegment(id, from, to, length, speedLimit, blocked != 0));
    }

    for (const auto& pair : sim.getCityMap().getAllIntersections())
        sim.addTrafficLight(pair.first, TrafficLight(pair.first, 10, 2, 8));
}

void FileManager::loadVehicles(const std::string& filename, Simulation& sim) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filename);

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string id, type, start, dest;
        if (!(iss >> id >> type >> start >> dest)) {
            std::cerr << "Warning: malformed vehicle line: " << line << "\n";
            continue;
        }

        std::shared_ptr<Vehicle> v;
        if      (type == "Car")       v = std::make_shared<Car>(id);
        else if (type == "Bus")       v = std::make_shared<Bus>(id);
        else if (type == "Truck")     v = std::make_shared<Truck>(id);
        else if (type == "Emergency") v = std::make_shared<EmergencyVehicle>(id);
        else {
            std::cerr << "Warning: unknown vehicle type '" << type << "' for " << id << "\n";
            continue;
        }

        v->setRoute({start, dest});
        sim.addVehicle(std::move(v));
    }
}

void FileManager::loadAccidents(const std::string& filename, Simulation& sim) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filename);

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string id, roadId, severityStr;
        int startTime, duration;
        if (!(iss >> id >> roadId >> severityStr >> startTime >> duration)) {
            std::cerr << "Warning: malformed accident line: " << line << "\n";
            continue;
        }

        AccidentSeverity sev;
        if      (severityStr == "Low")    sev = AccidentSeverity::LOW;
        else if (severityStr == "Medium") sev = AccidentSeverity::MEDIUM;
        else if (severityStr == "High")   sev = AccidentSeverity::HIGH;
        else {
            std::cerr << "Warning: unknown severity '" << severityStr << "'\n";
            continue;
        }

        sim.getAccidentManager().addAccident(
            std::make_unique<TrafficAccident>(id, roadId, sev, startTime, duration));
    }
}

void FileManager::saveResults(const std::string& filename, const Simulation& sim) {
    std::ofstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open output file: " + filename);
    sim.printStatus(file);
    std::cout << "Results saved to " << filename << "\n";
}
