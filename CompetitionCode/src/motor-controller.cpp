#include "motor-controller.h"

using namespace vex;
using namespace std;

const double WAIT_TIME = 25; // msec

void controlMotor(void *arg) {
  printf("controlmotor \n");
  MotorController *mc = (MotorController *)arg;
  double tau = mc->getTau();

  while (true) {
    double targetVal = mc->getTargetValue();
    double currentVal = mc->getControlledValue();
    double thresh = mc -> getMaxSpeedThresh();

    int speed = (int)((targetVal - currentVal) / tau);
    if (speed < -100 || (speed < 0 && fabs(targetVal - currentVal) > thresh))
      speed = -100;
    if (speed > 100 || (speed > 0 && fabs(targetVal - currentVal) > thresh))
      speed = 100;
    if(mc->getOverRide()) {
      speed = mc->getOverRidePercent();
    }

    if(mc->getEnabled()){
      mc->spinMotors(speed);
    }
    else{
      mc->spinMotors(0);
    }
    wait(WAIT_TIME, msec);
  }
}

MotorController::MotorController(motor *cm, pot* cP, double *tv,
                                 double tau) {


  controlledMotor = cm;
  controlledMotorGroup = (motor_group*) NULL;
  controlledPotV2 = NULL;
  controlledPot = cP;
  targetValue = tv;
  this->tau = tau;
  maxSpeedThresh = 1000000;

  enabled = false;
  
  new thread(controlMotor, this);
}

MotorController::MotorController(motor *cm, potV2* cPV2, double *tv,
                                 double tau) {


  controlledMotor = cm;
  controlledMotorGroup = (motor_group*) NULL;
  controlledPotV2 = cPV2;
  targetValue = tv;
  this->tau = tau;
  maxSpeedThresh = 1000000;

  enabled = false;
  
  new thread(controlMotor, this);
}

MotorController::MotorController(motor_group *cm, potV2 *cPV2, double *tv,
                                 double tau) {


  controlledMotor = NULL;
  controlledMotorGroup = cm;
  controlledPotV2 = cPV2;
  targetValue = tv;
  this->tau = tau;
  maxSpeedThresh = 1000000;

  enabled = false;
  
  new thread(controlMotor, this);
}

MotorController::MotorController(motor *cm, double *tv,
                                 double tau) {
  controlledMotor = cm;
  controlledMotorGroup = (motor_group*)NULL;
  targetValue = tv;
  this->tau = tau;
  maxSpeedThresh = 1000000;

  enabled = false;
  
  new thread(controlMotor, this);
}

void MotorController::setEnabled(bool b){
  enabled = b;
}

motor *MotorController::getControlledMotor() { return controlledMotor; }

//spins motors if they are a group or individual motoroooooo
void MotorController::spinMotors(int v){
  if(controlledMotor != NULL){
    controlledMotor->setVelocity(v, percent);
    controlledMotor->spin(fwd);
  }
  else{
    controlledMotorGroup->setVelocity(v, percent);
    controlledMotorGroup->spin(fwd);
  }
}

// returns controlledValue
double MotorController::getControlledValue() { 
    if(controlledPotV2 != NULL){
      return controlledPotV2->angle(degrees); 
    }
    else if(controlledPot != NULL){
      return controlledPot->angle(degrees);
    }
    else{
      return controlledMotor->velocity(percent);
    }
}

// returns targetValue
double MotorController::getTargetValue() { return *targetValue; }

// returns tau
double MotorController::getTau() { return tau; }

//returns enabled
bool MotorController::getEnabled(){ return enabled; }

void MotorController::setOverRideCycles(int n) { overRideCycles = n; }

void MotorController:: setOverRidePercent(int p){ overRidePercent = p;}

bool MotorController:: getOverRide(){
  if (overRideCycles > 0){
    overRideCycles--;
    return true;
  }
  return false;
}

int MotorController:: getOverRidePercent(){
  return overRidePercent;
}

double MotorController::getMaxSpeedThresh(){ return maxSpeedThresh;}

void MotorController:: setMaxSpeedThresh(double t){maxSpeedThresh = t; }
