/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
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
// TestJump             digital_in    D               
// AutoTest             digital_in    E               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;
using namespace std;

// A global instance of competition
competition Competition;

// define your global instances of motors and other devices here

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

  
  while(!Competition.isAutonomous() && !Competition.isDriverControl()){

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
  Brain.Screen.clearScreen();
  Brain.Timer.reset();
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
  Controller1.Screen.print("AUTO ");
  Controller1.Screen.print(selectedAuto);
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

//method to control controller screen. currently has temp monitoring, plan to implement timer. should do multithready stuff with it, if you hate yourself, like i do. :)
void controllerScreen(){
  double avgTemp;
  double hiTemp;
  
  int totalSecondsRemaining;
  int minutesRemaining;
  int secondsRemaining;
  

  bool toggle = false;

  string highestTempMotor; 
  
  motor motors[4] = {LeftFrontMotor, LeftBackMotor, RightFrontMotor, RightBackMotor};

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
    for(int i = 0; i<4; i++){
      if(motors[i].temperature(celsius) >= hiTemp){
        hiTemp = motors[i].temperature(percent);
        hiMotor = i;
      }
    }



    //Controller commands
    //time takes precedence, followed by temp warning, follwoed by everything else
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

        Controller1.Screen.print(" WARN");
      }
      toggle = !toggle;
    }
    else{
      Controller1.Screen.print(minutesRemaining);
      Controller1.Screen.print(":");
      Controller1.Screen.print(secondsRemaining);
      Controller1.Screen.setCursor(1,10);
      Controller1.Screen.print("AVG: ");
      Controller1.Screen.print(avgTemp);
      Controller1.Screen.newLine();
      Controller1.Screen.print("HI: ");
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

void usercontrol(void) {

  thread UIControl(controllerScreen);

  //clearing screen of anything printed in pre-auto
  Brain.Screen.clearScreen();


  //printing roboknights logo
  Brain.Screen.drawImageFromFile("RoboKnights logo 2019.png", 10, 10);

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
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(0,0);
    //initializing motorspeed variables
    leftMotorSpeed = Controller1.Axis3.position(percent);
    rightMotorSpeed = Controller1.Axis2.position(percent);

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

    if(Controller1.ButtonR1.pressing()){
      IntakeMotor.setVelocity(100, percent);
    }
    else if(Controller1.ButtonB.pressing()){
      IntakeMotor.setVelocity(-100, percent);
    }
    else{
      IntakeMotor.setVelocity(0, percent);
    }

    if(Controller1.ButtonL1.pressing()){
      LiftMotor.setVelocity(100, percent);
    }
    else if(Controller1.ButtonL2.pressing()){
      LiftMotor.setVelocity(-100, percent);
    }
    else{
      LiftMotor.setVelocity(0, percent);
    }

//todo: make this work
    if((Controller1.ButtonR2.pressing() != clampLast) && Controller1.ButtonR2.pressing()){
      clamp = !clamp;
    }

    if(Controller1.ButtonY.pressing()){
      MobileGoalMotor.setVelocity(100, percent);
    }
    else if(Controller1.ButtonDown.pressing()){
      MobileGoalMotor.setVelocity(-100, percent);
    }
    else{
      MobileGoalMotor.setVelocity(0, percent);
    }
    //telling motors to spin

    LeftBackMotor.spin(fwd);
    LeftFrontMotor.spin(fwd);
    RightBackMotor.spin(fwd);
    RightFrontMotor.spin(fwd);

    IntakeMotor.spin(fwd);
    LiftMotor.spin(fwd);
    MobileGoalMotor.spin(fwd);
    ClampPiston.set(clamp);
    Brain.Screen.print(clamp);


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

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}

