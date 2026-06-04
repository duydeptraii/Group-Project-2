#include "include/Simulation.hpp"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        Simulation sim;

        std::cout << "=== Smart City Traffic Simulation ===\n\n";
        std::cout << "Loading city data...\n";
        sim.loadFromFiles("data/roads.txt", "data/vehicles.txt", "data/accidents.txt");

        std::cout << "Initializing vehicle routes...\n";
        sim.initialize();

        int steps = 0;
        std::cout << "Enter number of simulation steps (e.g. 20): ";
        std::cin >> steps;
        if (steps <= 0) steps = 20;

        std::cout << "\nRunning simulation for " << steps << " steps...\n";
        sim.run(steps);

        std::cout << "\nSimulation complete.\n";
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
