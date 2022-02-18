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

    int speed = (int)((targetVal - currentVal) / tau);
    if (speed < -100)
      speed = -100;
    if (speed > 100)
      speed = 100;
    controlledMotor->setVelocity(speed, percent);
    controlledMotor->spin(fwd);

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
}

motor *MotorController::getControlledMotor() { return controlledMotor; }

// returns controlledValue
double MotorController::getControlledValue() { return controlledPot->angle(degrees); }

// returns targetValue
double MotorController::getTargetValue() { return *targetValue; }

// returns tau
double MotorController::getTau() { return tau; }
