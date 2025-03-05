# Garage Door System Simulation


## Introduction
This project simulates a **garage door control system** using the **Cadmium DEVS** modeling framework. It integrates multiple atomic and coupled models to simulate a **door access control system** with authentication, emergency handling, alarms, and lighting.

## Dependencies
This project assumes that you have Cadmium installed in a location accessible by the environment variable $CADMIUM.
_This dependency would be met by default if you are using the ARSLAB servers. To check, try `echo $CADMIUM` in the terminal_

## Build
To clone this project, run:
```sh
git clone https://github.com/Alobieda/GarageDoor.git
```
To build this project, run:
```sh
source build_sim.sh
```
__NOTE__: Every time you run build_sim.sh, the contents of `build/` and `bin/` will be replaced.



## Execute
To run the project, run:
```sh
./bin/sample_project
```

## Additional Testing
- **the system is being tested with a generator that only generate hardcoded input code.
- ** if you would like to test the emergency input, you have to change the vector in the generator from taking int to bool 

## Features
- **Code-based access control**: Users enter a passcode to unlock the door.
- **Emergency mode**: An emergency trigger overrides authentication and immediately unlocks the door.
- **Alarm system**: Unauthorized access attempts are tracked, and an alarm is triggered after multiple failures.
- **Automated lighting**: Indoor and outdoor lights turn on based on motion, authentication, and environmental conditions.
- **Test generator**: A test generator model provides automated inputs to the system for simulation testing.



## Model Descriptions
### **Atomic Models**
#### **codCont.hpp (Code Controller)**
- **Inputs**: `inputCode` (int), `openReq` (bool), `emergOpen` (bool)
- **Outputs**: `emer` (bool), `validCode` (bool), `invalidCode` (bool)
- **Function**: 
  - Processes user passcodes.
  - Triggers emergency mode if needed.
  - Validates correct/incorrect passcodes.

#### **counter.hpp (Failed Attempts Counter)**
- **Inputs**: `invalidCode` (bool)
- **Outputs**: `sendAlarm` (bool)
- **Function**:
  - Tracks failed access attempts.
  - Triggers an alarm after three consecutive failures within a time limit.

#### **door_timer.hpp (Door Auto-Close Timer)**
- **Inputs**: `validCode` (bool)
- **Outputs**: `doorOpen` (bool)
- **Function**:
  - Opens the door upon valid code entry.
  - Automatically closes the door after a delay.

#### **lightIn.hpp (Indoor Lighting Controller)**
- **Inputs**: `validCode` (bool)
- **Outputs**: `insideLightOn` (bool)
- **Function**:
  - Turns on the indoor lights when a valid code is entered.
  - Turns off after a predefined duration.

#### **lightOut.hpp (Outdoor Motion Sensor & Lighting Controller)**
- **Inputs**: `motion` (bool), `lightSensor` (bool)
- **Outputs**: `outLightOn` (bool)
- **Function**:
  - Activates the outdoor lights when motion is detected at night.
  - Turns off after a delay.

#### **alarmGenerator.hpp (Alarm System)**
- **Inputs**: `unauth_attempt` (bool), `emerg` (bool), `alarm_reset` (bool)
- **Outputs**: `minor_alarm` (bool), `major_alarm` (bool)
- **Function**:
  - Activates alarms based on failed attempts and emergency triggers.
  - Allows alarm resets.

### **Coupled Models**
#### **doorController.hpp (Door Controller Subsystem)**
- **Submodels**: `codCont`, `counter`, `door_timer`, `alarmGen`
- **Inputs**: `inputCode`, `emergOpen`, `openReq`, `alarmReset`
- **Outputs**: `doorOpen`, `minorAlarmOut`, `majorAlarmOut`
- **Function**:
  - Orchestrates authentication, door opening, alarms, and timing.

#### **doorSystem.hpp (Complete Door System)**
- **Submodels**: `doorController`, `lightIn`, `lightOut`
- **Inputs**: `inputCode`, `emergOpen`, `openReq`, `alarmReset`
- **Outputs**: `insideLightOn`, `outsideLightOn`, `doorOpen`
- **Function**:
  - Integrates all subsystems into a complete door security and lighting system.


## Authors
Developed by **Belal Alobieda 101170694*.

---


