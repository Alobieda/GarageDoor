#ifndef LIGHTOUT_HPP
#define LIGHTOUT_HPP

#include <cstring>
#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"


using namespace cadmium;

struct LightOutState{
    enum State{ON, OFF}state;
    bool isDayTime;
    bool isSecondTime;
    bool isLightOn;
    double sigma;

    LightOutState()
        :state(OFF), sigma(std::numeric_limits<double>::infinity()),
        isSecondTime(false), isDayTime(false){}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const LightOutState& state){
    out<< "State: " << (state.state == LightOutState:: OFF? "OFF  " : "ON  ");
    if(state.state == LightOutState:: ON){
        out << "The Light is " << (state.isLightOn? "ON  " : "OFF  ");
        out<< "it is a " << (state.isDayTime? "DayTime  " : "NightTime  ");
        out<< ", another code was  " <<(state.isSecondTime? "ENTERED  " : "NOT ENTERED  ");
        out<< "Sigma: "<< state.sigma ; 
    }
    return out;
}
#endif

class LightOut : public Atomic<LightOutState>{
public:
    Port<bool>motion;
    Port<bool>lightSensor;
    Port<bool>outLightOn;

    LightOut(const std::string& id)
        : Atomic<LightOutState>(id, LightOutState()){
            motion = addInPort<bool>("motion");
            lightSensor = addInPort<bool>("lightSensor");
            outLightOn = addOutPort<bool>("outLightPort");
        }
    
    //internalTransition 
    void internalTransition(LightOutState& state)const override{
        if(state.isSecondTime){
            state.isSecondTime = false;
            state.sigma = 120;
        }
        else{
            state.state = LightOutState:: OFF;
            state.sigma = std::numeric_limits<bool>::infinity();
        }
    }

    void externalTransition(LightOutState& state, double e)const override{
        if(state.sigma != std::numeric_limits<bool>::infinity()){
            state.sigma -= e;
        }

        if(!lightSensor->empty()){
            state.isDayTime = lightSensor->getBag().back();
        }
        
        if(!motion->empty()){
            if(!state.isDayTime){
                
                if(state.state == LightOutState::ON){
                    state.isSecondTime = true;
                    state.isLightOn = true;
                }
                else{
                    state.isLightOn = true;
                    state.state = LightOutState::ON;
                    state.sigma = 120;
                }
            }
        }
    }

    void output(const LightOutState& state)const override{
        outLightOn->addMessage(state.isLightOn);
    }

    [[nodiscard]] double timeAdvance(const LightOutState& state)const override{
        return state.sigma;
    }
};
#endif

