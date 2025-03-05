#ifndef ALARMGEN_HPP
#define ALARMGEN_HPP

#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct AlarmGenState {
    enum State { IDLE, ACTIVE } state;
    double sigma;
    bool ifminor;
    bool ifmajor;
    bool ifreset;

    AlarmGenState()
        : state(IDLE), sigma(std::numeric_limits<double>::infinity()),
          ifminor(false), ifmajor(false), ifreset(false) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const AlarmGenState& state) {
    out << "State: " << (state.state == AlarmGenState::ACTIVE ? "ACTIVE  " : "IDLE  ") << ", ";
    if(state.state == AlarmGenState::ACTIVE){
        out << "Minor Alarm: " << (state.ifminor ? "ON  " : "OFF  ") << ", ";
        out << "Major Alarm: " << (state.ifmajor ? "ON  " : "OFF  ") << ", ";
        out << "Reset: " << (state.ifreset ? "TRIGGERED  " : "NOT TRIGGERED  ") << ", ";
        out << "Sigma: " << state.sigma;
    }
    return out;
}
#endif

class AlarmGen : public Atomic<AlarmGenState> {
public:
    Port<bool> unauth_attempt;
    Port<bool> emerg;
    Port<bool> alarm_reset;
    Port<bool> minor_alarm;
    Port<bool> major_alarm;

    AlarmGen(const std::string& id)
        : Atomic<AlarmGenState>(id, AlarmGenState()){
            emerg = addInPort<bool>("emerg");
            unauth_attempt = addInPort<bool>("unauth_attempt");
            alarm_reset = addInPort<bool>("alarm_reset");
            minor_alarm = addOutPort<bool>("minor_alarm");
            major_alarm = addOutPort<bool>("major_alarm");
        }

   

    void internalTransition(AlarmGenState& state) const override {
        state.ifminor = false;
        state.ifmajor = false;
        state.ifreset = false;
        state.state = AlarmGenState::IDLE;
        state.sigma = std::numeric_limits<double>::infinity();
    }

    void externalTransition(AlarmGenState& state, double e) const override {
        if (state.sigma != std::numeric_limits<double>::infinity()) {
            state.sigma -= e;
        }

        if (!alarm_reset->empty() && alarm_reset->getBag().back()) {
            state.ifreset = true;
        }
        if (!unauth_attempt->empty() && unauth_attempt->getBag().back()) {
            state.ifminor = true;
        }
        if (!emerg->empty() && emerg->getBag().back()) {
            state.ifmajor = true;  
        }

        state.state = AlarmGenState::ACTIVE;
        state.sigma = 0;
    }

    void output(const AlarmGenState& state) const override {
        if (state.ifreset) {
            minor_alarm->addMessage(false);
            major_alarm->addMessage(false);
        } else if (state.ifmajor) {
            major_alarm->addMessage(true);
        } else if (state.ifminor) {
            minor_alarm->addMessage(true);
        }
    }

    [[nodiscard]] double timeAdvance(const AlarmGenState& state) const override {
        return state.sigma;
    }
};

#endif 
