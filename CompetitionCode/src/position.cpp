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

double prevXAccel = 0;
double prevYAccel = 0;

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

    xPos += (xVelocity * deltaTime) + (1/3) * prevXAccel * pow(deltaTime, 2) + (1/6) * xAccel * pow(deltaTime, 2);
    yPos += (yVelocity * deltaTime) + (1/3) * prevYAccel * pow(deltaTime, 2) + (1/6) * yAccel * pow(deltaTime, 2);   

    xVelocity = (xAccel + prevXAccel) / 2 * deltaTime;
    yVelocity = (yAccel + prevYAccel) / 2 * deltaTime; 

    prevXAccel = xAccel;
    prevYAccel = yAccel;

    printf("X Acceleration: %f Velocity: %f Position: %f", xAccel, xVelocity, xPos);
    printf("Y Acceleration: %f Velocity: %f Position: %f", yAccel, yVelocity, yPos);

    wait(deltaTime, units); 
  }

}