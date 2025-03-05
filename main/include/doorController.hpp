#ifndef DOOR_CONTROLLER_HPP
#define DOOR_CONTROLLER_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "counter.hpp"
#include "codCont.hpp"
#include "door_timer.hpp"
#include "alarmGenerator.hpp"  

using namespace cadmium;

class DoorController : public Coupled {
public:
    
    Port<int>  inputCode;   
    Port<bool> emergOpen;  
    Port<bool> openReq;     
    Port<bool> alarmReset;  

    
    Port<bool> doorOpen;        
    Port<bool> minorAlarmOut;   
    Port<bool> majorAlarmOut;   

    DoorController(const std::string& id) : Coupled(id) {
        
        inputCode  = addInPort<int>("inputCode");
        emergOpen  = addInPort<bool>("emergOpen");
        openReq    = addInPort<bool>("openReq");
        alarmReset = addInPort<bool>("alarmReset");  
        
        doorOpen       = addOutPort<bool>("doorOpen");
        minorAlarmOut  = addOutPort<bool>("minorAlarmOut");
        majorAlarmOut  = addOutPort<bool>("majorAlarmOut");

        
        auto counter1   = addComponent<counter>("counter");
        auto codeCont1  = addComponent<codCont>("codCont");
        auto doorTimer1 = addComponent<doorTimer>("doorTimer");
        auto alarmGen1  = addComponent<AlarmGen>("alarmGen");

        // 4) External Input Couplings (this->in -> submodel->in)
        addCoupling(this->inputCode,  codeCont1->inputCode);
        addCoupling(this->emergOpen,  codeCont1->emergOpen);
        addCoupling(this->openReq,    codeCont1->openReq);
        addCoupling(this->alarmReset ,alarmGen1->alarm_reset); 

        // 5) Internal Couplings (submodel->out -> submodel->in)
        addCoupling(codeCont1->validCode,   doorTimer1->validCode);
        addCoupling(codeCont1->invalidCode, counter1->invalidCode);
        addCoupling(codeCont1->emer,        alarmGen1->emerg);
        addCoupling(counter1->sendAlarm,    alarmGen1->unauth_attempt);

        // 6) External Output Couplings (submodel->out -> this->out)
        addCoupling(doorTimer1->doorOpen,      this->doorOpen);
        addCoupling(alarmGen1->minor_alarm,    this->minorAlarmOut);
        addCoupling(alarmGen1->major_alarm,    this->majorAlarmOut);
    }
};

#endif
