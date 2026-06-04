#include "../include/Road.hpp"
using namespace std;

RoadSegment::RoadSegment()
    : length(0.0), speedLimit(0.0), blocked(false),
      currentSpeedLimit(0.0), vehicleCount(0) {}

RoadSegment::RoadSegment(const string& id, const string& from, const string& to,
                         double length, double speedLimit, bool blocked)
    : id(id), fromId(from), toId(to), length(length), speedLimit(speedLimit),
      blocked(blocked), currentSpeedLimit(speedLimit), vehicleCount(0) {}
