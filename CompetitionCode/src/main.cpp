/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Andrew Goad, Katie Caldwell, JD Francois, David Ntim      */
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
// LeftFrontMotor       motor         19              
// LeftBackMotor        motor         9               
// RightFrontMotor      motor         12              
// RightBackMotor       motor         3               
// ClampPiston          digital_out   F               
// IntakeMotor          motor         18              
// LiftMotor            motor         4               
// MobileGoalMotor      motor         13              
// TestJump             digital_in    D               
// AutoTest             digital_in    E               
// MobileGoalSwitch     limit         C               
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


//declaring and initializing preauto flag
bool preauto = true;


void controllerScreen(){
  double avgTemp;
  double hiTemp;
  
  int totalSecondsRemaining;
  int minutesRemaining;
  int secondsRemaining;
  

  bool toggle = false;

  string highestTempMotor; 
  
  motor motors[5] = {LeftFrontMotor, LeftBackMotor, RightFrontMotor, RightBackMotor, MobileGoalMotor};

  int hiMotor = 0;
  int warningTemp = 55; //temperature at which the brain throttles control

  Brain.Timer.reset();

  

  while(true){
    //timer calculations
    totalSecondsRemaining = 105 - ((int) Brain.Timer.time(seconds));
    minutesRemaining = ((int) totalSecondsRemaining % 60);
    secondsRemaining = (int) totalSecondsRemaining - (minutesRemaining * 60);

    //calculating average temp of the 4 motors
    avgTemp = (LeftBackMotor.temperature(percent) + LeftFrontMotor.temperature(percent) + RightBackMotor.temperature(percent) + RightFrontMotor.temperature(percent)) / 4; 

    //calculating highest motor temp
    for(int i = 0; i<5; i++){
      if(motors[i].temperature(percent) >= hiTemp){
        hiTemp = motors[i].temperature(percent);
        hiMotor = i;
      }
    }



    //Controller commands
    //time takes precedence, followed by temp warning, follwoed by everything else
    Controller1.Screen.setCursor(0, 0);

    if(totalSecondsRemaining == 15){
      Controller1.rumble(rumbleShort);
      Controller1.Screen.print("TIME WARN");
    }
    else if(hiTemp > warningTemp){
      //rumbling controller if motor temps go above threshold
      Controller1.rumble(rumblePulse);
      if(!toggle){
        
        //i'm aware that this is absolutely disgusting but theres a quirk in the vexcode api that makes it necessary :(

        if (hiMotor == 0) {
          Controller1.Screen.print("LF");
        }
        else if(hiMotor == 1){
          Controller1.Screen.print("LB");
        }
        else if(hiMotor == 2){
          Controller1.Screen.print("RF");
        }
        else if(hiMotor == 3){
          Controller1.Screen.print("RB");
        }
        else if(hiMotor == 4){
          Controller1.Screen.print("MG");
        }
        Controller1.Screen.print(" WARN");

        Controller1.Screen.newLine();

        Controller1.Screen.print(motors[hiMotor].temperature(percent));
      }
      toggle = !toggle;
    }
    else{
      Controller1.Screen.print("TIME:");
      Controller1.Screen.print(minutesRemaining);
      Controller1.Screen.print(":");
      Controller1.Screen.print(secondsRemaining);
      Controller1.Screen.newLine();
      Controller1.Screen.print("AVG/HI:");
      Controller1.Screen.print(avgTemp);
      Controller1.Screen.print("/");
      Controller1.Screen.print(hiTemp);
      Controller1.Screen.newLine();
    }



    //waiting to avoid making the lcd look weird, or taking up all that sweet sweet cpu time
    //decrease value if you need the ui to update faster, quarter second should be fine
    wait(250, msec);

    //clearing screen to make room for next values
    Controller1.Screen.clearScreen();
  }
}

void pre_auton(void) {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  //declaring and initialzing variables
  selectedAuto = 0;
  bool selected = false;

  
  while(preauto){

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


    //test

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
  
  //updating flag to cause preauton method to exit
  preauto = false;

  Brain.Screen.print("Robot under autonomous control. Please stand clear.");
  Controller1.Screen.print("AUTO");

  new thread(controllerScreen);

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

  //updating flag to cause preauton method to exit
  preauto = false;

  //clearing screen of anything printed in pre-auto
  Brain.Screen.clearScreen();

  //declaring and initializing clamp variables
  bool clamp = false;
  bool clampLast = false;


  //default deadzone value 
  int deadzone = 3;

  //declaring motor speed vars
  int leftMotorSpeed = 0;
  int rightMotorSpeed = 0;

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

    if(Controller1.ButtonR1.pressing()){
      IntakeMotor.setVelocity(100, percent);
    }
    else if(Controller1.ButtonY.pressing()){
      IntakeMotor.setVelocity(-100, percent);
    }
    else{
      IntakeMotor.setVelocity(0, percent);
    }

    if(Controller1.ButtonL1.pressing() && !MobileGoalSwitch.pressing()){
      MobileGoalMotor.setVelocity(100, percent);
    }
    else if(Controller1.ButtonL2.pressing()){
      MobileGoalMotor.setVelocity(-100, percent);
    }
    else{
      MobileGoalMotor.setVelocity(0, percent);
    }

    //same as above
    if(abs(rightMotorSpeed) < deadzone) {
      RightBackMotor.setVelocity(0, percent);
      RightFrontMotor.setVelocity(0, percent);
    }
    else{
      RightBackMotor.setVelocity(-rightMotorSpeed, percent);
      RightFrontMotor.setVelocity(-rightMotorSpeed, percent);
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

    MobileGoalMotor.spin(reverse);

    IntakeMotor.spin(fwd);

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
  // check for test jumpers, if present run user control or autotest
  while (true) {
      //if the correct jumpers are in place and the competition switch is disconnected, activate the auto test mode or go directly to user control
    if(TestJump && AutoTest && !Competition.isCompetitionSwitch()){
      while(1){
        while(!Controller1.ButtonR2.pressing()){
          if(Controller1.ButtonA.pressing()){
            selectedAuto = 0;
          }
          else if(Controller1.ButtonB.pressing()){
            selectedAuto = 1;
          }
          else if(Controller1.ButtonX.pressing()){
            selectedAuto = 2;
          }
          else if(Controller1.ButtonY.pressing()){
            selectedAuto = 3;
          }
      }
    }
      autonomous();
    }
    else if(TestJump && !Competition.isCompetitionSwitch()){
      usercontrol();
    }
    wait(100, msec);
  }
}

