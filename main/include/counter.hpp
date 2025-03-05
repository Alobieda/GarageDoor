#ifndef COUNTER_HPP
#define COUNTER_HPP

#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"


using namespace cadmium;

struct counterState{
    enum State{IDLE, SEND_ALARM}state;
    double sigma;
    int count;
    double first_initial_time;
    double second_initial_time;
    double setTime = 120;

    counterState()
    : state(IDLE), sigma(std::numeric_limits<double>::infinity()),
    count(0), first_initial_time(0){}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream& out, const counterState& state){
    out << "State : " <<(state.state == counterState::IDLE? "IDLE     " : "SEND_ALARM   ");
    out << "Failed attampt :" << state.count ;
    return out;
}
#endif

class counter : public Atomic<counterState>{
public:
    Port<bool>invalidCode;
    Port<bool>sendAlarm;

    counter(const std::string& id)
        : Atomic<counterState>(id, counterState()){
            invalidCode = addInPort<bool>("invalidCode");
            sendAlarm = addOutPort<bool>("sendAlarm");
        }

    //internalTransition
    void internalTransition(counterState& state)const override{
        state.state = counterState::IDLE;
        state.count = 0;
        state.sigma = std::numeric_limits<double>::infinity();
    }

    //externalTransition
    void externalTransition(counterState& state, double e)const override{
        if(state.sigma != std::numeric_limits<double>::infinity()){
            state.sigma -= e;
        }

        if(!invalidCode->empty()){
            state.count += 1;
            if(state.count == 1){
                state.first_initial_time = e;
            }
            if(state.count == 2){
                state.second_initial_time = e;
            }
            else if(state.count == 3){
                double elepcedTime = e - state.first_initial_time;
                if(elepcedTime <= state.setTime){
                    state.state = counterState::SEND_ALARM;
                    state.count = 0 ;
                    state.sigma = 0;
                    state.first_initial_time = 0;
                }
                else {
                    double elepcedSecondTime = e - state.second_initial_time;
                    if(elepcedSecondTime <= state.setTime){
                        state.count = 2;
                        state.first_initial_time = e;
                    }
                    else{
                        state.count = 1;
                        state.first_initial_time = e;
                    }
                    
                }

            }
        }
    }

    //output
    void output(const counterState& state)const override{
        sendAlarm->addMessage(counterState::SEND_ALARM);
    }

    [[nodiscard]] double timeAdvance(const counterState& state)const override{
        return state.sigma;
    }
};
#endif





// #ifndef COUNTER_HPP
// #define COUNTER_HPP

// #include <limits>
// #include <iostream>
// #include "cadmium/modeling/devs/atomic.hpp"

// using namespace cadmium;

// struct counterState {
//     enum State { IDLE, SEND_ALARM } state;
//     double sigma;
//     int count;
//     double first_attempt_time;
//     double second_attempt_time;
//     static constexpr double setTime = 120.0; // Time threshold for alarm

//     counterState()
//         : state(IDLE), sigma(std::numeric_limits<double>::infinity()),
//           count(0), first_attempt_time(-1), second_attempt_time(-1) {}
// };

// #ifndef NO_LOGGING
// std::ostream& operator<<(std::ostream& out, const counterState& state) {
//     out << "State: " << (state.state == counterState::IDLE ? "IDLE" : "SEND_ALARM");
//     out << ", Failed Attempts: " << state.count;
//     return out;
// }
// #endif

// class counter : public Atomic<counterState> {
// public:
//     Port<bool> invalidCode;
//     Port<bool> sendAlarm;

//     counter(const std::string& id)
//         : Atomic<counterState>(id, counterState()) {
//         invalidCode = addInPort<bool>("invalidCode");
//         sendAlarm = addOutPort<bool>("sendAlarm");
//     }

//     // Internal Transition
//     void internalTransition(counterState& state) const override {
//         state.state = counterState::IDLE;
//         state.count = 0;
//         state.sigma = std::numeric_limits<double>::infinity();
//         state.first_attempt_time = -1;
//         state.second_attempt_time = -1;
//     }

//     // External Transition
//     void externalTransition(counterState& state, double e) const override {
//         if (state.sigma != std::numeric_limits<double>::infinity()) {
//             state.sigma -= e;
//         }

//         if (!invalidCode->empty() && invalidCode->getBag().back()) {
//             state.count++;

//             if (state.count == 1) {
//                 state.first_attempt_time = e;  // Track first invalid attempt
//             } else if (state.count == 2) {
//                 state.second_attempt_time = e; // Track second invalid attempt
//             } else if (state.count == 3) {
//                 double elapsedTime = e - state.first_attempt_time;
//                 if (elapsedTime <= state.setTime) {
//                     state.state = counterState::SEND_ALARM;
//                     state.sigma = 0;  // Trigger alarm immediately
//                 } else {
//                     // Shift second attempt to first, restart counter logic
//                     state.count = 2;
//                     state.first_attempt_time = state.second_attempt_time;
//                     state.second_attempt_time = e;
//                 }
//             }
//         }
//     }

//     // Output Function
//     void output(const counterState& state) const override {
//         if (state.state == counterState::SEND_ALARM) {
//             sendAlarm->addMessage(true);  // Send boolean instead of enum
//         }
//     }

//     // Time Advance
//     [[nodiscard]] double timeAdvance(const counterState& state) const override {
//         return state.sigma;
//     }
// };

// #endif // COUNTER_HPP

