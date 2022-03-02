#include "position.h"

//one g in meters per second per second
const double G_IN_MPSPS = 9.80665;

const double EPSILON = 1E-5;

const double REST_ACCEL = 0.5;

void startPositionMonitor(void * arg){
  PositionMonitor * daMonitor = (PositionMonitor *) arg;

  daMonitor->trackPosition();
}

void PositionMonitor::trackPosition(){


  while(true){
    //update value and convert
    xAccel = monitoredAccel->acceleration(xaxis) * G_IN_MPSPS;
    yAccel = monitoredAccel->acceleration(yaxis) * G_IN_MPSPS;

    if(fabs(xAccel) < REST_ACCEL){
      xAccel = 0;
    }

    if(fabs(yAccel) < REST_ACCEL){
      yAccel = 0;
    }

    xPos += (xVelocity * deltaTime) + (1.0/3) * prevXAccel * pow(deltaTime, 2) + (1.0/6) * xAccel * pow(deltaTime, 2);
    yPos += (yVelocity * deltaTime) + (1.0/3) * prevYAccel * pow(deltaTime, 2) + (1.0/6) * yAccel * pow(deltaTime, 2);   

    xVelocity += (xAccel + prevXAccel) / 2 * deltaTime;
    yVelocity += (yAccel + prevYAccel) / 2 * deltaTime; 

    prevXAccel = xAccel;
    prevYAccel = yAccel;

    printf("X Acceleration: %f Velocity: %f Position: %f \n", xAccel, xVelocity, xPos);
    printf("Y Acceleration: %f Velocity: %f Position: %f \n", yAccel, yVelocity, yPos);

    wait(deltaTime, units); 
  }
}

PositionMonitor::PositionMonitor(inertial* tA, double dT, timeUnits u){
  monitoredAccel = tA;
  deltaTime = dT;
  units = u;

  new thread(startPositionMonitor, this);
}

double PositionMonitor::getXPos(){
  return xPos;
}

double PositionMonitor::getYPos(){
  return yPos;
}

