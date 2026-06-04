#include "../include/Road.hpp"

RoadSegment::RoadSegment()
    : length(0.0), speedLimit(0.0), blocked(false),
      currentSpeedLimit(0.0), vehicleCount(0) {}

RoadSegment::RoadSegment(const std::string& id, const std::string& from, const std::string& to,
                         double length, double speedLimit, bool blocked)
    : id(id), fromId(from), toId(to), length(length), speedLimit(speedLimit),
      blocked(blocked), currentSpeedLimit(speedLimit), vehicleCount(0) {}
