#ifndef SAMPLE_TOP_HPP
#define SAMPLE_TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "lightIn.hpp"
#include "lightOut.hpp"
#include "doorController.hpp"
#include "generator.hpp"

using namespace cadmium;

class doorSystem : public Coupled {
public:
    Port<int>   InputCode;  
    Port<bool>  EmergOpen;
    Port<bool>  openReq;
    Port<bool>  AlarmReset;
    Port<bool> doorOpen;
    Port<bool> minor_alarm;
    Port<bool> major_alarm;
    Port<bool> inLight;
    Port<bool> outLight;


    doorSystem(const std::string& id) : Coupled(id) {

        InputCode  = addInPort<int>("outInputCode");
        EmergOpen  = addInPort<bool>("outEmergOpen");
        openReq    = addInPort<bool>("outOpenReq");
        AlarmReset = addInPort<bool>("outAlarmReset");

        doorOpen  = addOutPort<bool>("doorOpen");
        minor_alarm  = addOutPort<bool>("minor_alarm");
        major_alarm    = addOutPort<bool>("major_alarm");
        inLight = addOutPort<bool>("inLight");
        outLight = addOutPort<bool>("outLight");



        // 1) Instantiate submodels (just like in DoorController)
        auto lightIn        = addComponent<LightIn>("lightIn");
        auto lightOut       = addComponent<LightOut>("lightOut");
        auto doorController = addComponent<DoorController>("doorController");

        // 2) Add internal couplings
        //    (The exact ports you connect are up to your design.)
        addCoupling(doorController->doorOpen, lightIn->validCode);
        addCoupling(doorController->doorOpen, lightOut->motion);
        //addCoupling(doorController->minorAlarmOut, lightOut->lightSensor);
        addCoupling(doorController->majorAlarmOut, lightIn->validCode);
        addCoupling(doorController->majorAlarmOut, lightOut->motion);
        addCoupling(this->openReq, doorController->openReq);
        addCoupling(this->EmergOpen, doorController->emergOpen);
        addCoupling(this->InputCode, doorController->inputCode);
        addCoupling(this->AlarmReset, doorController->alarmReset);
    }
};

#endif  
