#pragma once
#include <string>
#include <queue>
#include <vector>

struct VehicleRef {
    int priority;
    std::string vehicleId;
    bool operator<(const VehicleRef& other) const {
        return priority < other.priority;
    }
};

class Intersection {
private:
    std::string id;
    std::priority_queue<VehicleRef> waitingQueue;

public:
    explicit Intersection(const std::string& id);

    const std::string& getId() const { return id; }
    void addWaitingVehicle(const std::string& vehicleId, int priority);
    std::string getNextVehicle();
    bool hasWaiting() const { return !waitingQueue.empty(); }
    int getWaitingCount() const { return static_cast<int>(waitingQueue.size()); }
    void clearQueue();
};
