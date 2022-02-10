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
// AutoTest             digital_in    E               
// MobileGoalSwitch     limit         C               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;
using namespace std;

// A global instance of competition
competition Competition;

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

//global variable changed in preauton function in order to control which auton is run
int selectedAuto = 0;


//declaring and initializing preauto flag
bool preauto = true;

int controllerCurve(int input, double curve){
  
  double dubInput = input;

  dubInput /= 100;

  dubInput = pow(dubInput, curve);

  dubInput *= 100;

  if(input >= 0){
    return (int)dubInput;
  }
  return (int)(fabs(dubInput) * -1);

}


void controllerScreen(){

  //declaring and initializing variables for temp control
  double avgTemp = 0;
  double hiTemp = 0;
  
  //delcaring and variables for timer
  int totalSecondsRemaining;
  int minutesRemaining;
  int secondsRemaining;
  
  motor motors[5] = {LeftFrontMotor, LeftBackMotor, RightFrontMotor, RightBackMotor, MobileGoalMotor};

  int hiMotor = 0;
  int warningTemp = 100; //temperature at which the brain throttles control

  Brain.Timer.reset();

  while(true){
    //timer calculations

    //subtracting seconds since brain timer reset from 105 (user control time in secoinds)
    totalSecondsRemaining = 105 - ((int) Brain.Timer.time(seconds));

    //splitting into minutes and seconds remaining for display
    minutesRemaining = (totalSecondsRemaining / 60);
    secondsRemaining = (totalSecondsRemaining - (minutesRemaining * 60));

    //calculating average temp of the 4 motors
    avgTemp = (LeftBackMotor.temperature(percent) + LeftFrontMotor.temperature(percent) + RightBackMotor.temperature(percent) + RightFrontMotor.temperature(percent)) / 4; 

    //calculating highest motor temp
    for(int i = 0; i<5; i++){
      if(motors[i].temperature(percent) >= hiTemp){
        hiTemp = motors[i].temperature(percent);
        hiMotor = i;
      }
    }



    //controller screen print commands
    //time takes precedence, followed by temp warning, follwoed by everything else
    Controller1.Screen.setCursor(0, 0);

    if(totalSecondsRemaining == 15){
      Controller1.rumble(rumbleShort);
      Controller1.Screen.print("TIME WARN");
    }
    else if(hiTemp > warningTemp){
      //rumbling controller if motor temps go above threshold
      Controller1.rumble(rumblePulse);
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
    else{
      //make sure that the correct number of digits is printed for seconds
      if(secondsRemaining < 10){
        Controller1.Screen.print("TIME: %d:0%d", minutesRemaining, secondsRemaining);
      }
      else{
        Controller1.Screen.print("TIME: %d:%d", minutesRemaining, secondsRemaining);
      }
      //print temperature values
      Controller1.Screen.newLine();
      Controller1.Screen.print("AVG/HI: %.2f:%.2f", avgTemp, hiTemp);
    }



    //waiting to avoid making the lcd look weird, or taking up all that sweet sweet cpu time
    //decrease value if you need the ui to update faster, quarter second should be fine
    wait(1, sec);

    //clearing screen to make room for next values
    Controller1.Screen.clearScreen();
  }
}

void pre_auton(void) {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  //initializing selected auto to 0
  selectedAuto = 0;

  //true when autonomous is "locked in" false when still selecting
  bool selected = false;

  //preauto flag turns false when usercontrol or autonomous begins
  while(preauto){

    //if menucycle is pressed and auton is not locked in
    if(MenuCycle.pressing() && !selected){
      //increment auton, rollover to 0
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

    wait(300, msec);

    //clearing screen to make room for next cycle
    Brain.Screen.clearScreen();
  }
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

  while(true){
    switch(selectedAuto){
      case 0:
        Brain.Screen.print("0");
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
  new thread(controllerScreen);

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

    //LeftMotor: Left Stick with deadzone
    if(abs(leftMotorSpeed) < deadzone) {
      //stopping if joystick within deadzone
      LeftBackMotor.setVelocity(0, percent);
      LeftFrontMotor.setVelocity(0, percent);
      //sets motors to brake mode
      LeftBackMotor.stop(hold);
      LeftFrontMotor.stop(hold);
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
      RightBackMotor.setVelocity(rightMotorSpeed, percent);
      RightFrontMotor.setVelocity(rightMotorSpeed, percent);
    }

    //Clamp: R2 toggles
    if(Controller1.ButtonR2.pressing() && !clampLast){
      clamp = !clamp;
    }

    //MobileGoalMotor: L1: in, Y: out
    if(Controller1.ButtonL1.pressing()){
      MobileGoalMotor.setVelocity(-50, percent);
    }
    else if(Controller1.ButtonL2.pressing()){
      MobileGoalMotor.setVelocity(100, percent);
    }
    else{
      MobileGoalMotor.setVelocity(0, percent);
    }
    
    //spinning motors and activating hydraulics
    LeftBackMotor.spin(fwd);
    LeftFrontMotor.spin(fwd);
    RightBackMotor.spin(fwd);
    RightFrontMotor.spin(fwd);

    MobileGoalMotor.spin(fwd);

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
    if(AutoTest && !Competition.isCompetitionSwitch()){
      while(1){
        //use letter buttons to select autonomous, run with r2
        //afterwards, select again
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
    wait(100, msec);
  }
}