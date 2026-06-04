#pragma once
#include <string>
#include <ostream>
using namespace std;

enum class LightState { GREEN, YELLOW, RED };

class TrafficLight {
private:
    string intersectionId;
    LightState state;
    int greenDuration;
    int yellowDuration;
    int redDuration;
    int timer;

public:
    TrafficLight() : intersectionId(""), state(LightState::GREEN),
                     greenDuration(10), yellowDuration(2), redDuration(8), timer(0) {}
    TrafficLight(const string& intersectionId, int green = 10, int yellow = 2, int red = 8);

    void update();
    LightState getState() const { return state; }
    string getStateString() const;
    const string& getIntersectionId() const { return intersectionId; }
    bool isGreen() const { return state == LightState::GREEN; }
    void printStatus(ostream& out) const;
};
