#include "vex.h"

#ifndef POSITION_H
#define POSITION_H

class PositionMonitor {
  inertial* monitoredAccel;
  double deltaTime;
  timeUnits units;

  public:
    PositionMonitor(inertial* tA, double dT, timeUnits u);
    void trackPosition();

    double getYPos();
    double getXPos();
};

#endif // POSITION_H