#ifndef DOOR_TIMER_HPP
#define DOOR_TIMER_HPP


#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"


struct doorTimerState{
    enum State{OPEN, CLOSE}state;
    double sigma;
    bool isOpen;
    bool isSecond;

    doorTimerState()
    : state(CLOSE), sigma(std::numeric_limits<double>::infinity()),
    isOpen(false), isSecond(false){}
};

#ifndef NO_LOGGING

std::ostream& operator<<(std::ostream &out, const doorTimerState& state){
    out << "State :" << (state.state == doorTimerState::CLOSE? "CLOSE  " : "OPEN  ");
    if(state.state == doorTimerState::OPEN){
        out << "The door is "<<(state.isOpen? "OPEN  " : "CLOSE  ");
        out<< "the timer is "<<(state.isSecond? "UPDATED  " : "NOT UPDATED  ");
        out<< "sigma: " <<state.sigma ;
    }
    return out;
}
#endif

class doorTimer : public  Atomic<doorTimerState>{
public :
    Port<bool>validCode;
    Port<bool>doorOpen;

    doorTimer(const std::string& id)
        : Atomic<doorTimerState>(id, doorTimerState()){
            validCode = addInPort<bool>("validCode");
            doorOpen = addOutPort<bool>("doorOpen");
        }


    //internalTransition
    void internalTransition(doorTimerState& state)const override{
        if(state.isSecond){
            state.isSecond = false;
            state.sigma = 15;
        }
        else{
            state.state = doorTimerState::CLOSE;
            state.sigma = std::numeric_limits<double>::infinity();
        }
    }


    //externalTransition
    void externalTransition(doorTimerState& state, double e)const override{
        if(state.sigma != std::numeric_limits<double>::infinity()){
            state.sigma -= e;
        }

        if(!validCode->empty()){
            if(state.state == doorTimerState::CLOSE){
                state.state = doorTimerState:: OPEN;
                state.isOpen = true;
                state.sigma = 15;
            }
            else{
                state.isSecond = true;
            }
        }
    }

    //output
    void output(const doorTimerState& state)const override{
        doorOpen->addMessage(doorTimerState::OPEN);
    }

    [[nodiscard]] double timeAdvance(const doorTimerState& state)const override{
        return state.sigma;
    }
};
#endif

