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
using namespace std;

void FileManager::loadRoads(const string& filename, Simulation& sim) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file: " + filename);
    }

    string line;
    while (getline(file, line)) {
        // Skip empty lines and comment lines starting with '#'
        if (line.empty() || line[0] == '#') {
            continue;
        }

        istringstream iss(line);
        string id, from, to;
        double length, speedLimit;
        int blocked;

        if (!(iss >> id >> from >> to >> length >> speedLimit >> blocked)) {
            cerr << "Warning: skipping malformed road line: " << line << "\n";
            continue;
        }

        bool isBlocked = (blocked != 0);
        RoadSegment road(id, from, to, length, speedLimit, isBlocked);
        sim.getCityMap().addRoad(road);
    }

    // Create one traffic light per intersection
    unordered_map<string, Intersection>& intersections = sim.getCityMap().getAllIntersections();
    for (pair<const string, Intersection>& entry : intersections) {
        TrafficLight tl(entry.first, 10, 2, 8);
        sim.addTrafficLight(entry.first, tl);
    }
}

void FileManager::loadVehicles(const string& filename, Simulation& sim) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file: " + filename);
    }

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        istringstream iss(line);
        string id, type, start, dest;

        if (!(iss >> id >> type >> start >> dest)) {
            cerr << "Warning: skipping malformed vehicle line: " << line << "\n";
            continue;
        }

        shared_ptr<Vehicle> v;
        if (type == "Car") {
            v = make_shared<Car>(id);
        } else if (type == "Bus") {
            v = make_shared<Bus>(id);
        } else if (type == "Truck") {
            v = make_shared<Truck>(id);
        } else if (type == "Emergency") {
            v = make_shared<EmergencyVehicle>(id);
        } else {
            cerr << "Warning: unknown vehicle type '" << type << "' for " << id << "\n";
            continue;
        }

        // Store start and destination temporarily; full route is planned in initialize()
        vector<string> tempRoute;
        tempRoute.push_back(start);
        tempRoute.push_back(dest);
        v->setRoute(tempRoute);

        sim.addVehicle(v);
    }
}

void FileManager::loadAccidents(const string& filename, Simulation& sim) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file: " + filename);
    }

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        istringstream iss(line);
        string id, roadId, severityStr;
        int startTime, duration;

        if (!(iss >> id >> roadId >> severityStr >> startTime >> duration)) {
            cerr << "Warning: skipping malformed accident line: " << line << "\n";
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
            cerr << "Warning: unknown severity '" << severityStr << "'\n";
            continue;
        }

        // unique_ptr is used because AccidentManager takes ownership of the accident object
        unique_ptr<TrafficAccident> accident =
            make_unique<TrafficAccident>(id, roadId, sev, startTime, duration);

        sim.getAccidentManager().addAccident(move(accident));
    }
}

void FileManager::saveResults(const string& filename, const Simulation& sim) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open output file: " + filename);
    }
    sim.printStatus(file);
    cout << "Results saved to " << filename << "\n";
}
