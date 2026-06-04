#pragma once
#include <string>

struct RoadSegment {
    std::string id;
    std::string fromId;
    std::string toId;
    double length;
    double speedLimit;
    bool blocked;
    double currentSpeedLimit;
    int vehicleCount;

    RoadSegment();
    RoadSegment(const std::string& id, const std::string& from, const std::string& to,
                double length, double speedLimit, bool blocked);

    bool isCongested() const { return vehicleCount >= 3; }
    void reduceSpeed(double factor) { currentSpeedLimit = speedLimit * factor; }
    void restoreSpeed() { currentSpeedLimit = speedLimit; }
};
