#include "../include/TrafficLight.hpp"
using namespace std;

TrafficLight::TrafficLight(const string& intersectionId, int green, int yellow, int red)
    : intersectionId(intersectionId), state(LightState::GREEN),
      greenDuration(green), yellowDuration(yellow), redDuration(red), timer(0) {}

void TrafficLight::update() {
    timer++;
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

string TrafficLight::getStateString() const {
    if (state == LightState::GREEN)  return "GREEN";
    if (state == LightState::YELLOW) return "YELLOW";
    if (state == LightState::RED)    return "RED";
    return "UNKNOWN";
}

void TrafficLight::printStatus(ostream& out) const {
    out << "Traffic Light " << intersectionId << ": " << getStateString() << "\n";
}
