#pragma once
#include <string>
#include <queue>
#include <vector>
using namespace std;

struct VehicleRef {
    int priority;
    string vehicleId;
    bool operator<(const VehicleRef& other) const {
        return priority < other.priority;
    }
};

class Intersection {
private:
    string id;
    priority_queue<VehicleRef> waitingQueue;

public:
    Intersection() : id("") {}
    explicit Intersection(const string& id);

    const string& getId() const { return id; }
    void addWaitingVehicle(const string& vehicleId, int priority);
    string getNextVehicle();
    bool hasWaiting() const { return !waitingQueue.empty(); }
    int getWaitingCount() const { return (int)waitingQueue.size(); }
    void clearQueue();
};
