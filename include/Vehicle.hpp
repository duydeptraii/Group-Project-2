#pragma once
#include <string>
#include <vector>
#include <ostream>
using namespace std;

enum class VehicleStatus { MOVING, WAITING, COMPLETED, REROUTING };

class Vehicle {
protected:
    string id;
    double speed;
    int priority;
    vector<string> route;
    int routeIndex;
    string currentRoadId;
    double positionOnRoad;
    VehicleStatus status;
    double totalWaitTime;

public:
    Vehicle(const string& id, double speed, int priority);
    virtual ~Vehicle() = default;

    virtual string getType() const = 0;

    string getId() const { return id; }
    double getSpeed() const { return speed; }
    int getPriority() const { return priority; }

    void setRoute(const vector<string>& r);
    const vector<string>& getRoute() const { return route; }
    int getRouteIndex() const { return routeIndex; }
    void advanceRouteIndex() { ++routeIndex; }

    void setCurrentRoad(const string& roadId) { currentRoadId = roadId; }
    const string& getCurrentRoad() const { return currentRoadId; }

    void setPosition(double pos) { positionOnRoad = pos; }
    double getPosition() const { return positionOnRoad; }

    void setStatus(VehicleStatus s) { status = s; }
    VehicleStatus getStatus() const { return status; }

    void addWaitTime(double t) { totalWaitTime += t; }
    double getTotalWaitTime() const { return totalWaitTime; }

    string getCurrentIntersection() const;
    string getDestination() const;

    void printStatus(ostream& out) const;
};

class Car : public Vehicle {
public:
    explicit Car(const string& id);
    string getType() const override { return "Car"; }
};

class Bus : public Vehicle {
public:
    explicit Bus(const string& id);
    string getType() const override { return "Bus"; }
};

class Truck : public Vehicle {
public:
    explicit Truck(const string& id);
    string getType() const override { return "Truck"; }
};

class EmergencyVehicle : public Vehicle {
public:
    explicit EmergencyVehicle(const string& id);
    string getType() const override { return "Emergency Vehicle"; }
};
