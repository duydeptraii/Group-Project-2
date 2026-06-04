#pragma once
#include <string>
#include <ostream>
using namespace std;

enum class AccidentSeverity { LOW, MEDIUM, HIGH };

class TrafficAccident {
private:
    string id;
    string roadId;
    AccidentSeverity severity;
    int startTime;
    int duration;
    bool active;
    bool blocksRoad;

public:
    TrafficAccident(const string& id, const string& roadId,
                    AccidentSeverity severity, int startTime, int duration);

    void update(int currentTime);

    const string& getId() const { return id; }
    const string& getRoadId() const { return roadId; }
    AccidentSeverity getSeverity() const { return severity; }
    bool isActive() const { return active; }
    bool doesBlockRoad() const { return blocksRoad && active; }
    double getSpeedFactor() const;
    string getSeverityString() const;
    void printStatus(ostream& out) const;
};
