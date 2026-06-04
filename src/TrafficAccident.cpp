#include "../include/TrafficAccident.hpp"
using namespace std;

TrafficAccident::TrafficAccident(const string& id, const string& roadId,
                                 AccidentSeverity severity, int startTime, int duration)
    : id(id), roadId(roadId), severity(severity), startTime(startTime),
      duration(duration), active(false), blocksRoad(false) {}

void TrafficAccident::update(int currentTime) {
    active    = (currentTime >= startTime && currentTime < startTime + duration);
    blocksRoad = active && (severity == AccidentSeverity::HIGH);
}

double TrafficAccident::getSpeedFactor() const {
    if (!active) {
        return 1.0;
    }
    if (severity == AccidentSeverity::LOW)    return 0.8;
    if (severity == AccidentSeverity::MEDIUM) return 0.5;
    if (severity == AccidentSeverity::HIGH)   return 0.0;
    return 1.0;
}

string TrafficAccident::getSeverityString() const {
    if (severity == AccidentSeverity::LOW)    return "Low";
    if (severity == AccidentSeverity::MEDIUM) return "Medium";
    if (severity == AccidentSeverity::HIGH)   return "High";
    return "Unknown";
}

void TrafficAccident::printStatus(ostream& out) const {
    if (!active) {
        return;
    }
    out << "  Accident " << id << " on Road " << roadId << "\n";
    out << "  Severity: " << getSeverityString() << "\n";
    out << "  Road Status: " << (blocksRoad ? "BLOCKED" : "REDUCED SPEED") << "\n";
}
