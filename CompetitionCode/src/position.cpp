#include "position.h"

inertial* trackedAccel;
double deltaTime;

//one g in meters per second per second
const double G_IN_MPSPS = 9.80665;

PositionMonitor::PositionMonitor(inertial* tA, double dT, timeUnits u){
  trackedAccel = tA;
  deltaTime = dT;
  units = u;
}

void PositionMonitor::trackPosition(){
  double xPos = 0;
  double yPos = 0;
  double xAccel = 0;
  double yAccel = 0;


  while(true){
    //update value and convert
    xAccel = trackedAccel->acceleration(xaxis) / G_IN_MPSPS;

    xPos += (xAccel) / deltaTime;
    wait(deltaTime, units); 
  }
}