#include "position.h"

inertial* trackedAccel;
double deltaTime = 10;
timeUnits u = msec;

//one g in meters per second per second
const double G_IN_MPSPS = 9.80665;

double xVelocity = 0;
double yVelocity = 0;
double xPos = 0;
double yPos = 0;
double xAccel = 0;
double yAccel = 0;

PositionMonitor::PositionMonitor(inertial* tA, double dT, timeUnits u){
  trackedAccel = tA;
  deltaTime = dT;
  units = u;
}

double PositionMonitor::getXPos(){
  return xPos;
}

double PositionMonitor::getYPos(){
  return yPos;
}

void PositionMonitor::trackPosition(){


  while(true){
    //update value and convert
    xAccel = trackedAccel->acceleration(xaxis) / G_IN_MPSPS;
    yAccel = trackedAccel->acceleration(yaxis) / G_IN_MPSPS;

    xVelocity = xVelocity * deltaTime + (xAccel / 2) * pow(deltaTime, 2);
    yVelocity = yVelocity * deltaTime + (xAccel / 2) * pow(deltaTime, 2);

    xPos += xVelocity;
    yPos += yVelocity;
    wait(deltaTime, units); 
  }

}