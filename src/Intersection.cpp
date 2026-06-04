#include "../include/Intersection.hpp"

Intersection::Intersection(const std::string& id) : id(id) {}

void Intersection::addWaitingVehicle(const std::string& vehicleId, int priority) {
    waitingQueue.push({priority, vehicleId});
}

std::string Intersection::getNextVehicle() {
    if (waitingQueue.empty()) return "";
    VehicleRef top = waitingQueue.top();
    waitingQueue.pop();
    return top.vehicleId;
}

void Intersection::clearQueue() {
    while (!waitingQueue.empty()) waitingQueue.pop();
}
