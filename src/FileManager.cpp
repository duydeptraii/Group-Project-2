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
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comment lines starting with '#'
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string id, from, to;
        double length, speedLimit;
        int blocked;

        if (!(iss >> id >> from >> to >> length >> speedLimit >> blocked)) {
            std::cerr << "Warning: skipping malformed road line: " << line << "\n";
            continue;
        }

        bool isBlocked = (blocked != 0);
        RoadSegment road(id, from, to, length, speedLimit, isBlocked);
        sim.getCityMap().addRoad(road);
    }

    // Create one traffic light per intersection
    std::unordered_map<std::string, Intersection>& intersections = sim.getCityMap().getAllIntersections();
    for (std::pair<const std::string, Intersection>& entry : intersections) {
        TrafficLight tl(entry.first, 10, 2, 8);
        sim.addTrafficLight(entry.first, tl);
    }
}

void FileManager::loadVehicles(const std::string& filename, Simulation& sim) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string id, type, start, dest;

        if (!(iss >> id >> type >> start >> dest)) {
            std::cerr << "Warning: skipping malformed vehicle line: " << line << "\n";
            continue;
        }

        std::shared_ptr<Vehicle> v;
        if (type == "Car") {
            v = std::make_shared<Car>(id);
        } else if (type == "Bus") {
            v = std::make_shared<Bus>(id);
        } else if (type == "Truck") {
            v = std::make_shared<Truck>(id);
        } else if (type == "Emergency") {
            v = std::make_shared<EmergencyVehicle>(id);
        } else {
            std::cerr << "Warning: unknown vehicle type '" << type << "' for " << id << "\n";
            continue;
        }

        // Store start and destination temporarily; full route planned in initialize()
        std::vector<std::string> tempRoute;
        tempRoute.push_back(start);
        tempRoute.push_back(dest);
        v->setRoute(tempRoute);

        sim.addVehicle(v);
    }
}

void FileManager::loadAccidents(const std::string& filename, Simulation& sim) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string id, roadId, severityStr;
        int startTime, duration;

        if (!(iss >> id >> roadId >> severityStr >> startTime >> duration)) {
            std::cerr << "Warning: skipping malformed accident line: " << line << "\n";
            continue;
        }

        AccidentSeverity sev;
        if (severityStr == "Low") {
            sev = AccidentSeverity::LOW;
        } else if (severityStr == "Medium") {
            sev = AccidentSeverity::MEDIUM;
        } else if (severityStr == "High") {
            sev = AccidentSeverity::HIGH;
        } else {
            std::cerr << "Warning: unknown severity '" << severityStr << "'\n";
            continue;
        }

        // unique_ptr is used because AccidentManager takes ownership
        std::unique_ptr<TrafficAccident> accident =
            std::make_unique<TrafficAccident>(id, roadId, sev, startTime, duration);

        sim.getAccidentManager().addAccident(std::move(accident));
    }
}

void FileManager::saveResults(const std::string& filename, const Simulation& sim) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open output file: " + filename);
    }
    sim.printStatus(file);
    std::cout << "Results saved to " << filename << "\n";
}
