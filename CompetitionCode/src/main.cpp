/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Andrew Goad, Katie Caldwell                               */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Competition Template                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// MenuCycle            limit         A               
// MenuSelect           limit         B               
// LeftFrontMotor       motor         1               
// LeftBackMotor        motor         2               
// RightFrontMotor      motor         9               
// RightBackMotor       motor         10              
// ClampPiston          digital_out   C               
// IntakeMotor          motor         11              
// LiftMotor            motor         12              
// MobileGoalMotor      motor         13              
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;
using namespace std;

// A global instance of competition
competition Competition;

// define your global instances of motors and other devices here
//katie was here
/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/
int selectedAuto;

void pre_auton(void) {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  //declaring and initialzing variables
  selectedAuto = 0;
  bool selected = false;

  
  while(true){

    if(MenuCycle.pressing() && !selected){
      if(selectedAuto > 2){
        selectedAuto = 0;
      }
      else{
        selectedAuto++;
      }
    }
    if(MenuSelect.pressing()){
      selected = !selected;
    }

    //graphic printing

    if(selected && selectedAuto != 0){
      Brain.Screen.setPenColor(white);
      Brain.Screen.drawRectangle(0, 0, 120, 120);
    }
    else if(selectedAuto == 0){
      Brain.Screen.setPenColor(yellow);
      Brain.Screen.drawRectangle(0, 0, 120, 120);
    }
    else{
      Brain.Screen.setPenColor(blue);
      Brain.Screen.drawRectangle(0, 0, 120, 120);
    }
    
    if(selected && selectedAuto != 1){
      Brain.Screen.setPenColor(white);
      Brain.Screen.drawRectangle(0, 136, 120, 120);
    }
    else if(selectedAuto == 1){
      Brain.Screen.setPenColor(yellow);
      Brain.Screen.drawRectangle(0, 136, 120, 120);      
    }
    else{
      Brain.Screen.setPenColor(blue);
      Brain.Screen.drawRectangle(0, 136, 120, 120);
    }
    
    if(selected && selectedAuto != 2){
      Brain.Screen.setPenColor(white);
      Brain.Screen.drawRectangle(136, 136, 120, 120);
    }
    else if(selectedAuto == 2){
      Brain.Screen.setPenColor(yellow);
      Brain.Screen.drawRectangle(136, 136, 120, 120);
    }
    else{
      Brain.Screen.setPenColor(red);
      Brain.Screen.drawRectangle(136, 136, 120, 120);
    }
    
    if(selected && selectedAuto != 3){
      Brain.Screen.setPenColor(white);
      Brain.Screen.drawRectangle(136, 0, 120, 120);
    }
    else if(selectedAuto == 3){
      Brain.Screen.setPenColor(yellow);
      Brain.Screen.drawRectangle(136, 0, 120, 120);
    }
    else{
      Brain.Screen.setPenColor(red);
      Brain.Screen.drawRectangle(136, 0, 120, 120);
    }
    //all hail king boolean

    //wait statement
    wait(300, msec);

    //clearing screen to make room for next cycle
    Brain.Screen.clearScreen();
  }
  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................

  Brain.Screen.print("Robot under autonomous control. Please stand clear.");
  Controller1.Screen.print("AUTO");
//still being tested, not ready for competition yet
while(true){
  switch(selectedAuto){
    case 0:
      Brain.Screen.print("Running Autonomous 0");
      LeftBackMotor.spin(fwd);
      LeftFrontMotor.spin(fwd);
      RightBackMotor.spin(reverse);
      RightBackMotor.spin(reverse);
      break;
    
    case 1:
      Brain.Screen.print("1");
      break;

    case 2:
      Brain.Screen.print("2");
      break;

    case 3:
      Brain.Screen.print("3");
      break;

    default:
      Brain.Screen.print("error");
      break;
  }
  Brain.Screen.clearScreen();
  }


}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {
  //clearing screen of anything printed in pre-auto
  Brain.Screen.clearScreen();

  //declaring and initializing clamp variables
  bool clamp = false;
  bool clampLast = false;


  //default deadzone value 
  //want this to be as low as possible without any drift
  //test by printing input from the stick when its totally neutral and set this as one above the highest number displayed
  int deadzone = 3;

  //declaring motor speed vars
  int leftMotorSpeed;
  int rightMotorSpeed;

  // User control code here, inside the loop 
  while (true) {

    //initializing motorspeed variables
    leftMotorSpeed = Controller1.Axis3.position(percent);
    rightMotorSpeed = Controller1.Axis2.position(percent);

    //test
    //checking deadzone
    if(abs(leftMotorSpeed) < deadzone) {
      //stopping if joystick within deadzone
      LeftBackMotor.setVelocity(0, percent);
      LeftFrontMotor.setVelocity(0, percent);
    }
    else{
      //setting motor velocity

      LeftBackMotor.setVelocity(leftMotorSpeed, percent);
      LeftFrontMotor.setVelocity(leftMotorSpeed, percent);
    }

    //same as above
    if(abs(rightMotorSpeed) < deadzone) {
      RightBackMotor.setVelocity(0, percent);
      RightFrontMotor.setVelocity(0, percent);

    }
    else{
      RightBackMotor.setVelocity(rightMotorSpeed, percent);
      RightFrontMotor.setVelocity(rightMotorSpeed, percent);
    }

    //toggle clamp control variable
    if((Controller1.ButtonR2.pressing() != clampLast) && Controller1.ButtonR2.pressing()){
      clamp = !clamp;
    }
    
    //spinning motors and activating hydraulics

    LeftBackMotor.spin(fwd);
    LeftFrontMotor.spin(fwd);
    RightBackMotor.spin(fwd);
    RightFrontMotor.spin(fwd);

    ClampPiston.set(clamp);

    //update clamplast so inputs arent counted multiple times
    clampLast = Controller1.ButtonR2.pressing();
    wait(25, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
