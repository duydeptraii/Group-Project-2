#pragma once
#include <string>
using namespace std;

struct RoadSegment {
    string id;
    string fromId;
    string toId;
    double length;
    double speedLimit;
    bool blocked;
    double currentSpeedLimit;
    int vehicleCount;

    RoadSegment();
    RoadSegment(const string& id, const string& from, const string& to,
                double length, double speedLimit, bool blocked);

    bool isCongested() const { return vehicleCount >= 3; }
    void reduceSpeed(double factor) { currentSpeedLimit = speedLimit * factor; }
    void restoreSpeed() { currentSpeedLimit = speedLimit; }
};
