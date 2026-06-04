#pragma once
#include <string>
#include <ostream>

enum class AccidentSeverity { LOW, MEDIUM, HIGH };

class TrafficAccident {
private:
    std::string id;
    std::string roadId;
    AccidentSeverity severity;
    int startTime;
    int duration;
    bool active;
    bool blocksRoad;

public:
    TrafficAccident(const std::string& id, const std::string& roadId,
                    AccidentSeverity severity, int startTime, int duration);

    void update(int currentTime);

    const std::string& getId() const { return id; }
    const std::string& getRoadId() const { return roadId; }
    AccidentSeverity getSeverity() const { return severity; }
    bool isActive() const { return active; }
    bool doesBlockRoad() const { return blocksRoad && active; }
    double getSpeedFactor() const;
    std::string getSeverityString() const;
    void printStatus(std::ostream& out) const;
};
