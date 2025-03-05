#ifndef LIGHTIN_HPP
#define LIGHTIN_HPP

#include <cstring>
#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;


struct LightInState{
    enum State {ON, OFF} state;
    double sigma;
    bool isLightOn;
    bool isSecondCode;

    LightInState()
        : state(OFF), sigma(std::numeric_limits<double>::infinity()),
          isLightOn(false), isSecondCode(false){}  
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const LightInState& state){
    out << "State: " << (state.state == LightInState::ON? "ON  " : "OFF  ");
    if(state.state == LightInState::ON){
        out << "The Light is " << (state.isLightOn? "ON  " : "OFF  ");
        out << ", was another code entered ? " << (state.isSecondCode? "YES  " : "NO  ");
    }
    return out;
} 

#endif


class LightIn : public Atomic<LightInState>{
public:
    Port<bool>validCode;
    Port<bool>insideLightOn;

    //Constructor
    LightIn(const std::string& id)
    : Atomic<LightInState>(id, LightInState()){
        validCode = addInPort<bool>("validCode");
        insideLightOn = addOutPort<bool>("InsideLightOn");
    }

    //Internal transition
    void internalTransition(LightInState& state) const override{
        if(state.isSecondCode){
            state.isSecondCode = false; 
            state.sigma = 300;
        }
        else{
            state.state = LightInState::OFF;
            state.sigma = std::numeric_limits<double>::infinity();
        }
    }

    //external transition
    void externalTransition(LightInState& state, double e) const override{
        if(state.sigma != std::numeric_limits<double>::infinity()){
            state.sigma -= e;
        }

        if(! validCode->empty()){
            if(state.isLightOn){
                state.isSecondCode = true;
                state.isLightOn = true;
            }
            else{
                state.state = LightInState::ON;
                state.isLightOn = true;
                state.sigma = 300;
            }

        }
    }


    //output transition
    void output(const LightInState& state) const override{
        insideLightOn->addMessage(state.isLightOn);
    }

    [[nodiscard]] double timeAdvance(const LightInState& state)const override{
        return state.sigma;
    }
};   
#endif