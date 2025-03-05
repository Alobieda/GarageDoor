#ifndef TOP_MODEL_HPP
#define TOP_MODEL_HPP

#include "cadmium/modeling/devs/coupled.hpp"


#include "test_generator.hpp"   
#include "doorSystem.hpp"        

using namespace cadmium;

class TopModel : public Coupled {
public:
    Port<bool> doorOpen;
    Port<bool> in_light;
    Port<bool> out_light;
    Port<bool> minor_alarm;
    Port<bool> major_alarm;

    TopModel(const std::string &id) : Coupled(id) {

        doorOpen = addOutPort<bool>("doorOpen");
        in_light = addOutPort<bool>("in_light");
        out_light = addOutPort<bool>("out_light");
        major_alarm = addOutPort<bool>("major_alarm");
        minor_alarm = addOutPort<bool>("minor_alarm");

        auto generator  = addComponent<testGenerator>("testGenerator");
        auto system     = addComponent<doorSystem>("doorSystem");

        
        addCoupling(generator->out,  system->InputCode);
       

        addCoupling(system->doorOpen, this->doorOpen);
        addCoupling(system->minor_alarm, this->minor_alarm);
        addCoupling(system->major_alarm, this->major_alarm);
        addCoupling(system->inLight, this->in_light);
        addCoupling(system->outLight, this->out_light);

 
    }
};

#endif 
