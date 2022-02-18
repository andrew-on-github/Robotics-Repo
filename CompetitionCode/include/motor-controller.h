// using namespace vex;
#include "vex.h"

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

class MotorController {
    motor* controlledMotor;
    double* controlledValue;
    double* targetValue;
    double tau;

  public:
    MotorController(motor* cm, double* cv, double* tv, double tau);
    
    //accessors
    //returns pointer to motor
    motor* getControlledMotor();
    double getControlledValue();
    double getTargetValue();
    double getTau();
};

void controlMotor(MotorController mc)

#endif // MOTOR_CONTROLLER_H