#pragma once
#include <string>
#include <vector>
#include <ostream>

enum class VehicleStatus { MOVING, WAITING, COMPLETED, REROUTING };

class Vehicle {
protected:
    std::string id;
    double speed;
    int priority;
    std::vector<std::string> route;
    int routeIndex;
    std::string currentRoadId;
    double positionOnRoad;
    VehicleStatus status;
    double totalWaitTime;

public:
    Vehicle(const std::string& id, double speed, int priority);
    virtual ~Vehicle() = default;

    virtual std::string getType() const = 0;

    std::string getId() const { return id; }
    double getSpeed() const { return speed; }
    int getPriority() const { return priority; }

    void setRoute(const std::vector<std::string>& r);
    const std::vector<std::string>& getRoute() const { return route; }
    int getRouteIndex() const { return routeIndex; }
    void advanceRouteIndex() { ++routeIndex; }

    void setCurrentRoad(const std::string& roadId) { currentRoadId = roadId; }
    const std::string& getCurrentRoad() const { return currentRoadId; }

    void setPosition(double pos) { positionOnRoad = pos; }
    double getPosition() const { return positionOnRoad; }

    void setStatus(VehicleStatus s) { status = s; }
    VehicleStatus getStatus() const { return status; }

    void addWaitTime(double t) { totalWaitTime += t; }
    double getTotalWaitTime() const { return totalWaitTime; }

    std::string getCurrentIntersection() const;
    std::string getDestination() const;

    void printStatus(std::ostream& out) const;
};

class Car : public Vehicle {
public:
    explicit Car(const std::string& id);
    std::string getType() const override { return "Car"; }
};

class Bus : public Vehicle {
public:
    explicit Bus(const std::string& id);
    std::string getType() const override { return "Bus"; }
};

class Truck : public Vehicle {
public:
    explicit Truck(const std::string& id);
    std::string getType() const override { return "Truck"; }
};

class EmergencyVehicle : public Vehicle {
public:
    explicit EmergencyVehicle(const std::string& id);
    std::string getType() const override { return "Emergency Vehicle"; }
};
