#include "../include/Vehicle.hpp"
#include <iostream>

Vehicle::Vehicle(const std::string& id, double speed, int priority)
    : id(id), speed(speed), priority(priority), routeIndex(0),
      positionOnRoad(0.0), status(VehicleStatus::MOVING), totalWaitTime(0.0) {}

void Vehicle::setRoute(const std::vector<std::string>& r) {
    route = r;
    routeIndex = 0;
    positionOnRoad = 0.0;
}

std::string Vehicle::getCurrentIntersection() const {
    if (routeIndex < static_cast<int>(route.size()))
        return route[routeIndex];
    return "";
}

std::string Vehicle::getDestination() const {
    if (!route.empty()) return route.back();
    return "";
}

void Vehicle::printStatus(std::ostream& out) const {
    out << "Vehicle " << id << ":\n";
    out << "  Type: " << getType() << "\n";
    out << "  Speed: " << speed << " km/h\n";
    out << "  Priority: " << priority << "\n";
    out << "  Current Road: " << (currentRoadId.empty() ? "None" : currentRoadId) << "\n";
}

Car::Car(const std::string& id) : Vehicle(id, 60.0, 1) {}

Bus::Bus(const std::string& id) : Vehicle(id, 40.0, 1) {}

Truck::Truck(const std::string& id) : Vehicle(id, 30.0, 1) {}

EmergencyVehicle::EmergencyVehicle(const std::string& id) : Vehicle(id, 80.0, 3) {}
