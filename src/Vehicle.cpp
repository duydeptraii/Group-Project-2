#include "../include/Vehicle.hpp"
#include <iostream>
using namespace std;

Vehicle::Vehicle(const string& id, double speed, int priority)
    : id(id), speed(speed), priority(priority), routeIndex(0),
      positionOnRoad(0.0), status(VehicleStatus::MOVING), totalWaitTime(0.0) {}

void Vehicle::setRoute(const vector<string>& r) {
    route = r;
    routeIndex = 0;
    positionOnRoad = 0.0;
}

string Vehicle::getCurrentIntersection() const {
    if (routeIndex < (int)route.size()) {
        return route[routeIndex];
    }
    return "";
}

string Vehicle::getDestination() const {
    if (!route.empty()) {
        return route.back();
    }
    return "";
}

void Vehicle::printStatus(ostream& out) const {
    out << "Vehicle " << id << ":\n";
    out << "  Type: " << getType() << "\n";
    out << "  Speed: " << speed << " km/h\n";
    out << "  Priority: " << priority << "\n";
    out << "  Current Road: " << (currentRoadId.empty() ? "None" : currentRoadId) << "\n";
}

Car::Car(const string& id) : Vehicle(id, 60.0, 1) {}

Bus::Bus(const string& id) : Vehicle(id, 40.0, 1) {}

Truck::Truck(const string& id) : Vehicle(id, 30.0, 1) {}

EmergencyVehicle::EmergencyVehicle(const string& id) : Vehicle(id, 80.0, 3) {}
