#include "../include/Intersection.hpp"
using namespace std;

Intersection::Intersection(const string& id) : id(id) {}

void Intersection::addWaitingVehicle(const string& vehicleId, int priority) {
    VehicleRef ref;
    ref.priority  = priority;
    ref.vehicleId = vehicleId;
    waitingQueue.push(ref);
}

string Intersection::getNextVehicle() {
    if (waitingQueue.empty()) {
        return "";
    }
    VehicleRef top = waitingQueue.top();
    waitingQueue.pop();
    return top.vehicleId;
}

void Intersection::clearQueue() {
    while (!waitingQueue.empty()) {
        waitingQueue.pop();
    }
}
