#include "include/Simulation.hpp"
#include <iostream>
#include <stdexcept>
using namespace std;

int main() {
    try {
        Simulation sim;

        cout << "=== Smart City Traffic Simulation ===\n\n";
        cout << "Loading city data...\n";
        sim.loadFromFiles("data/roads.txt", "data/vehicles.txt", "data/accidents.txt");

        cout << "Initializing vehicle routes...\n";
        sim.initialize();

        int steps = 0;
        cout << "Enter number of simulation steps (e.g. 20): ";
        cin >> steps;
        if (steps <= 0) steps = 20;

        cout << "\nRunning simulation for " << steps << " steps...\n";
        sim.run(steps);

        cout << "\nSimulation complete.\n";
    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
