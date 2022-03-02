#include "vex.h"

#ifndef POSITION_H
#define POSITION_H

class PositionMonitor {
  inertial* monitoredAccel;
  double deltaTime;
  timeUnits units;

  double xVelocity;
  double yVelocity;

  double xPos;
  double yPos;

  double xAccel;
  double yAccel;

  double prevXAccel;
  double prevYAccel;

  public:
    PositionMonitor(inertial* tA, double dT, timeUnits u);
    void trackPosition();

    double getYPos();
    double getXPos();
};

#endif // POSITION_H