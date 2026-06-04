#include "../include/TrafficLight.hpp"

TrafficLight::TrafficLight(const std::string& intersectionId, int green, int yellow, int red)
    : intersectionId(intersectionId), state(LightState::GREEN),
      greenDuration(green), yellowDuration(yellow), redDuration(red), timer(0) {}

void TrafficLight::update() {
    ++timer;
    if (state == LightState::GREEN && timer >= greenDuration) {
        state = LightState::YELLOW;
        timer = 0;
    } else if (state == LightState::YELLOW && timer >= yellowDuration) {
        state = LightState::RED;
        timer = 0;
    } else if (state == LightState::RED && timer >= redDuration) {
        state = LightState::GREEN;
        timer = 0;
    }
}

std::string TrafficLight::getStateString() const {
    switch (state) {
        case LightState::GREEN:  return "GREEN";
        case LightState::YELLOW: return "YELLOW";
        case LightState::RED:    return "RED";
    }
    return "UNKNOWN";
}

void TrafficLight::printStatus(std::ostream& out) const {
    out << "Traffic Light " << intersectionId << ": " << getStateString() << "\n";
}
