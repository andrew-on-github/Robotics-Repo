// using namespace vex;
#include "vex.h"

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

class MotorController {
    motor* controlledMotor;
    motor_group* controlledMotorGroup;
    pot* controlledPot;
    potV2* controlledPotV2; 
    double* targetValue;
    double tau;
    bool enabled;
    int overRideCycles;
    int overRidePercent;
    double maxSpeedThresh;

  public:
    MotorController(motor* cm, pot* cP, double* tv, double tau);
    MotorController(motor* cm, potV2* cPV2, double* tv, double tau);
    MotorController(motor_group* cm, potV2* cPV2, double* tv, double tau);

    
    void spinMotors(int v);

    //accessors
    //returns pointer to motor
    motor* getControlledMotor();
    double getControlledValue();
    double getTargetValue();
    double getTau();
    void setEnabled(bool);
    bool getEnabled();
    void setOverRideCycles(int);
    void setOverRidePercent(int);
    bool getOverRide();
    int getOverRidePercent();
    double getMaxSpeedThresh();
    void setMaxSpeedThresh(double);

};

void controlMotor(MotorController mc);

#endif // MOTOR_CONTROLLER_H
