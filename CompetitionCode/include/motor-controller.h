// using namespace vex;
#include "vex.h"

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

class MotorController {
    motor* controlledMotor;
    potV2* controlledPot; 
    double* targetValue;
    double tau;
    bool enabled;
    int overRideCycles;
    int overRidePercent;
    double maxSpeedThresh;

  public:
    MotorController(motor* cm, potV2* cP, double* tv, double tau);
    
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