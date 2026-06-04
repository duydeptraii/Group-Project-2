#include "../include/TrafficAccident.hpp"

TrafficAccident::TrafficAccident(const std::string& id, const std::string& roadId,
                                 AccidentSeverity severity, int startTime, int duration)
    : id(id), roadId(roadId), severity(severity), startTime(startTime),
      duration(duration), active(false), blocksRoad(false) {}

void TrafficAccident::update(int currentTime) {
    active = (currentTime >= startTime && currentTime < startTime + duration);
    blocksRoad = active && (severity == AccidentSeverity::HIGH);
}

double TrafficAccident::getSpeedFactor() const {
    if (!active) return 1.0;
    switch (severity) {
        case AccidentSeverity::LOW:    return 0.8;
        case AccidentSeverity::MEDIUM: return 0.5;
        case AccidentSeverity::HIGH:   return 0.0;
    }
    return 1.0;
}

std::string TrafficAccident::getSeverityString() const {
    switch (severity) {
        case AccidentSeverity::LOW:    return "Low";
        case AccidentSeverity::MEDIUM: return "Medium";
        case AccidentSeverity::HIGH:   return "High";
    }
    return "Unknown";
}

void TrafficAccident::printStatus(std::ostream& out) const {
    if (!active) return;
    out << "  Accident " << id << " on Road " << roadId << "\n";
    out << "  Severity: " << getSeverityString() << "\n";
    out << "  Road Status: " << (blocksRoad ? "BLOCKED" : "REDUCED SPEED") << "\n";
}
