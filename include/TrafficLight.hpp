#pragma once
#include <string>
#include <ostream>

enum class LightState { GREEN, YELLOW, RED };

class TrafficLight {
private:
    std::string intersectionId;
    LightState state;
    int greenDuration;
    int yellowDuration;
    int redDuration;
    int timer;

public:
    TrafficLight(const std::string& intersectionId, int green = 10, int yellow = 2, int red = 8);

    void update();
    LightState getState() const { return state; }
    std::string getStateString() const;
    const std::string& getIntersectionId() const { return intersectionId; }
    bool isGreen() const { return state == LightState::GREEN; }
    void printStatus(std::ostream& out) const;
};
