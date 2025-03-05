#ifndef CODCONT_HPP
#define CODCONT_HPP

#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct codContState{
    enum State {IDLE, ACTIVE} state;
    double sigma;
    bool ifValid;
    bool ifUrgent;
    bool ifInvalid;
    int passCode;

    codContState()
    : state(IDLE), sigma(std::numeric_limits<double>::infinity()),
    ifValid(false), ifUrgent(false), ifInvalid(false), passCode(1234){}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream& out, const codContState& state){
    out << "State :" <<(state.state == codContState::IDLE? "IDLE    " : "ACTIVE   ");
    if(state.state == codContState::ACTIVE){
        out << "the CODE entered is "<< (!state.ifValid? "NOT VALID   " : "VALID  ");
        out << "the emeregency button "<< (!state.ifUrgent? "WAS NOT PRESSED  " : "WAS PRESSED  ");
        out << "sigma : " << state.sigma;
    }
    return out;
} 
#endif


class codCont: public Atomic<codContState>{
public:
    Port<bool>emer;
    Port<bool>validCode;
    Port<int>inputCode;
    Port<bool>openReq;
    Port<bool>emergOpen;
    Port<bool>invalidCode;

    codCont(const std::string& id)
        : Atomic<codContState>(id, codContState()){
            emer = addOutPort<bool>("emer");
            validCode = addOutPort<bool>("validCode");
            inputCode = addInPort<int>("inputCode");
            openReq = addInPort<bool>("openReq");
            emergOpen = addInPort<bool>("emergOpen");
            invalidCode = addOutPort<bool>("invalidCode");
        }

    //internalTransition
    void internalTransition(codContState& state) const override {
        state.ifInvalid = false;
        state.ifUrgent = false;
        state.ifValid = false;
        state.state = codContState::IDLE;
        state.sigma = std::numeric_limits<double>::infinity();
    }

    void externalTransition(codContState& state, double e) const override {
        if (state.sigma != std::numeric_limits<double>::infinity()) {
            state.sigma -= e;
        }

        if (!emergOpen->empty()) {
            bool check = emergOpen->getBag().back();
            if(check){ 
                state.ifUrgent = true;
            }
        }
        else if (!openReq->empty()) {
            bool check = openReq->getBag().back();
            if(check){
                state.ifValid = true;
            }
        }
        else if (!inputCode->empty()) {
            int incomingCode = inputCode->getBag().back();

            if (incomingCode == state.passCode) {
                state.ifValid = true;
            } else {
                state.ifInvalid = true;
            }
        }

        state.state = codContState::ACTIVE;
        state.sigma = 0;
    }

    void output(const codContState& state) const override {
        if (state.ifUrgent) {
            emer->addMessage(true);
            validCode->addMessage(true);
        }
        else if (state.ifValid) {
            validCode->addMessage(true);
        }
        else if (state.ifInvalid) {
            invalidCode->addMessage(true);
        }
    }

    [[nodiscard]] double timeAdvance(const codContState& state) const override {
        return state.sigma;
    }
};

#endif 

