#ifndef DOOR_TEST_GENERATOR_HPP
#define DOOR_TEST_GENERATOR_HPP

#include <random>
#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct DoorTestGenState {
    int messages_sent;
    double sigma;
    bool toggle;

    DoorTestGenState()
        : messages_sent(0),
          sigma(2.0),   
          toggle(false) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &os, const DoorTestGenState &st) {
    os << "{messages_sent=" << st.messages_sent
       << ", sigma=" << st.sigma
       << ", toggle=" << (st.toggle ? "true" : "false")
       << "}";
    return os;
}
#endif


class DoorTestGenerator : public Atomic<DoorTestGenState> {
public:
    // Output ports
    Port<int>   outInputCode;  
    Port<bool>  outEmergOpen;
    Port<bool>  outOpenReq;
    Port<bool>  outAlarmReset;

    // Constructor
    DoorTestGenerator(const std::string &id)
      : Atomic<DoorTestGenState>(id, DoorTestGenState()) {

        outInputCode  = addOutPort<int>("outInputCode");
        outEmergOpen  = addOutPort<bool>("outEmergOpen");
        outOpenReq    = addOutPort<bool>("outOpenReq");
        outAlarmReset = addOutPort<bool>("outAlarmReset");
    }

    
    void internalTransition(DoorTestGenState &s) const override {
        s.messages_sent++;
        s.toggle = !s.toggle;

        if (s.messages_sent < 10) {
            s.sigma = 2.0;  // next output in 2s
        } else {
            s.sigma = std::numeric_limits<double>::infinity(); // done
        }
    }

    // EXTERNAL TRANSITION:
    void externalTransition(DoorTestGenState &s, double e) const override {
        // no external inputs, do nothing
    }


    void output(const DoorTestGenState &s) const override {
        if (s.messages_sent < 10) {
            if (s.toggle) {
            
                outInputCode->addMessage(1234);  
                outOpenReq->addMessage(true);    
            } else {
               
                outEmergOpen->addMessage(true);
                outAlarmReset->addMessage(true);
            }
        }
    }

    // TIME ADVANCE:
    [[nodiscard]] double timeAdvance(const DoorTestGenState &s) const override {
        return s.sigma;
    }
};

#endif 
