#include "motor-controller.h"

using namespace vex;
using namespace std;

const double WAIT_TIME = 25; // msec

void controlMotor(void *arg) {
  MotorController *mc = (MotorController *)arg;
  double tau = mc->getTau();
  motor *controlledMotor = mc->getControlledMotor();

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
      controlledMotor->setVelocity(speed, percent);
      //controlledMotor->spin(fwd);
    }
    printf("%p %f %f %d \n", arg, targetVal, currentVal, speed);
    wait(WAIT_TIME, msec);
  }
}

MotorController::MotorController(motor *cm, potV2 *cP, double *tv,
                                 double tau) {

  new thread(controlMotor, this);

  controlledMotor = cm;
  controlledPot = cP;
  targetValue = tv;
  this->tau = tau;
  maxSpeedThresh = -1;

  enabled = false;
}

void MotorController::setEnabled(bool b){
  enabled = b;
}

motor *MotorController::getControlledMotor() { return controlledMotor; }

// returns controlledValue
double MotorController::getControlledValue() { return controlledPot->angle(degrees); }

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
