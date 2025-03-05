#include <vector>
#include <limits>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

// Define the state for the generator.
struct generatorState {
    double sigma;                  // Time until next event
    std::vector<int> testInputs;  // Pre-defined sequence of test inputs
    int index;                     // Current index in the sequence
};


#ifndef NO_LOGGING

#include <ostream>

std::ostream& operator<<(std::ostream &out, const generatorState &state) {
    // Print a label for the state
    out << "generatorState: ";
    // Print the sigma value
    out << "sigma=" << state.sigma << ", ";
    // Print the current index
    out << "index=" << state.index << ", ";
    // Print all test inputs
    return out;
}

#endif


// Define a simple generator atomic model.
class testGenerator : public Atomic<generatorState> {
public:
    // One output port: it sends a boolean to the counter model.
    Port<int> out;

    // Constructor accepts an id and a vector of test inputs.
    testGenerator(const std::string &id) 
        : Atomic<generatorState>(id, generatorState{0.0, std::vector<int>{1212, 1234, 1235, 1111, 2222}, 0}) {
        out = addOutPort<int>("out");
    }

    // Internal transition: after outputting a value, move to the next one.
    void internalTransition(generatorState &state) const override {
        state.index++;
        // If there are more inputs, schedule the next event after a fixed delay (e.g., 1 time unit)
        if(state.index < state.testInputs.size()) {
            state.sigma = 1.0;
        } else {
            // Otherwise, no more events
            state.sigma = std::numeric_limits<double>::infinity();
        }
    }

    // External transition: this model does not react to external events.
    void externalTransition(generatorState &state, double e) const override {
        state.sigma -= e;
    }

    // Output function: send the current test input value.
    void output(const generatorState &state) const override {
        if(state.index < state.testInputs.size()){
            out->addMessage(state.testInputs[state.index]);
        }
    }

    // Time advance: return the time until the next scheduled event.
    [[nodiscard]] double timeAdvance(const generatorState &state) const override {
        return state.sigma;
    }
};
