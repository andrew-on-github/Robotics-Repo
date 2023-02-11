//git 35X2022
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Andrew Goad, Katie Caldwell, JD Francois, David Ntim,     */
/*                  Jack Cataldo                                              */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Competition Template                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// MenuCycle            limit         D               
// MenuSelect           limit         E               
// LeftFrontMotor       motor         11              
// LeftBackMotor        motor         17              
// RightFrontMotor      motor         1               
// RightBackMotor       motor         19              
// FlywheelMotorLeft    motor         8               
// FlywheelMotorRight   motor         9               
// IntakeMotor          motor         2               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
#include "motor-controller.h"

using namespace vex;
using namespace std;

// A global instance of competition
competition Competition;

vex::motor_group LeftMotorGroup(LeftFrontMotor, LeftBackMotor);
vex::motor_group RightMotorGroup(RightFrontMotor, RightBackMotor);

vex::motor_group DriveMotorGroup(LeftFrontMotor, LeftBackMotor, RightFrontMotor, RightBackMotor);

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

//global variable changed in preauton function in order to control which auton is run
int selectedAuto = 0;

//time before brakes activate in msec
const int BRAKING_TIME = 750;

//difference between controller thumbsticks before they lock together
const int CONTROLLER_MATCHING_THRESHOLD = 25;

//speed the motors run at when doing fine tuning control
const int FINE_TUNING_SPEED = 7;

//forward and backward fine tuning control additive constant
const int FWD_BWD_ADD = 4;

//speed at which the motors run during aimAdjustment
const double CURVE = 1.7; 

//var to count amount of time reamining before breaking
int brakingTimeRemaining = BRAKING_TIME;

//amount of time in teh user control portion of the match in seconds
const int USERCONTROL_TIME_SECONDS = 105;

// flywheel spinning speed in percentages
const int FLYWHEEL_SPEED = 60;

//intake spinning speed
const int INTAKE_SPEED = 100;

//default deadzone value 
//want this to be as low as possible without any drift
//test by printing input from the stick when its totally neutral and set this as one above the highest number displayed
const int DEADZONE = 0;

//declaring and initializing preauto flag, set to false when pre autonomous is exited
bool preauto = true;
double controllerCurve(int input, double curve){
  
  double dubInput = fabs((double)input);

  dubInput /= 100;

  dubInput = pow(dubInput, curve);

  dubInput *= 100;

  if(input >= 0){
    dubInput = (int)dubInput;
  }
  else{
    dubInput = -1 * ((int)(dubInput));
  }

  return dubInput;
}

void controllerScreen(){

  //declaring and initializing variables for temp control
  double avgTemp = 0;
  double hiTemp = 0;
  
  //delcaring and variables for timer
  int totalSecondsRemaining;
  int minutesRemaining;
  int secondsRemaining;
  
  //be sure to adjust
  motor* motors[4] = {&LeftFrontMotor, &LeftBackMotor, &RightFrontMotor, &RightBackMotor};

  motor* hiMotor = 0;
  const int WARNING_TEMP = 65; //temperature at which the brain throttles control

  Brain.Timer.reset();
  Controller1.Screen.clearScreen();

  while(true){
    //timer calculations

    //subtracting seconds since brain timer reset from 105 (user control time in seconds)
    totalSecondsRemaining = USERCONTROL_TIME_SECONDS - ((int) Brain.Timer.time(seconds));

    //splitting into minutes and seconds remaining for display
    if(totalSecondsRemaining > 0){
      minutesRemaining = (totalSecondsRemaining / 60);
      secondsRemaining = (totalSecondsRemaining - (minutesRemaining * 60));
    }
    else{
      minutesRemaining = 0;
      secondsRemaining = 0;
    }

    //calculating average temp of the 4 motors
    int motorsSize = 0;
    int total = 0;
    for(motor* currentMotor : motors){
      total += currentMotor->temperature(percent);
      motorsSize++;
    }

    avgTemp = total / motorsSize;

    //calculating highest motor temp
    for(motor* currentMotor : motors){
      if(currentMotor->temperature(percent) >= hiTemp){
        hiTemp = currentMotor->temperature(percent);
        hiMotor = currentMotor;
      }
    }

    //controller screen print commands
    //time takes precedence, followed by temp warning, followed by everything else
    Controller1.Screen.setCursor(0, 0);

    if(totalSecondsRemaining == 15){
      Controller1.rumble(rumbleShort);
      Controller1.Screen.print("TIME WARN");
    }
    else if(hiTemp > WARNING_TEMP){
      //rumbling controller if motor temps go above threshold
      Controller1.rumble(rumblePulse);
      //i'm aware that this is absolutely disgusting but theres a quirk in the vexcode api that makes it necessary :(
      if (hiMotor == motors[0]) {
        Controller1.Screen.print("LF");
      }
      else if(hiMotor == motors[1]){
        Controller1.Screen.print("LB");
      }
      else if(hiMotor == motors[2]){
        Controller1.Screen.print("RF");
      }
      else if(hiMotor == motors[3]){
        Controller1.Screen.print("RB");
      }
      Controller1.Screen.print(" WARN");
      Controller1.Screen.newLine();
      Controller1.Screen.print("%.2f °C", hiMotor->temperature(celsius));
    }
    else{
      //make sure that the correct number of digits is printed for seconds
      Controller1.Screen.print("TIME: %d:%d", minutesRemaining, secondsRemaining);
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

  //declaring and initializing last cycle variables
  bool menuSelectLast = false;
  bool menuCycleLast = false;

  //true when autonomous is "locked in" false when still selecting
  bool selected = false;

  //double buffering
  Brain.Screen.render(true, false);

  //preauto flag turns false when usercontrol or autonomous begins
  while(preauto){
    //if menucycle is pressed and auton is not locked in
    if((MenuCycle.pressing() && !menuCycleLast) && !selected){
      //increment auton, rollover to 0
      if(selectedAuto >= 3){
        selectedAuto = 0;
      }
      else{
        selectedAuto++;
      }
    }
    if(MenuSelect.pressing() && !menuSelectLast){
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

    //setting last cycle variables
    menuCycleLast = MenuCycle.pressing();
    menuSelectLast = MenuSelect.pressing();
    
    Brain.Screen.render();
    
    //wait statement
    wait(5, msec);


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

  Brain.Screen.print("Running Autonomous No. ");
  Controller1.Screen.print("AUTO");

    switch(selectedAuto){
      case 0:
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

  //declaring motor speed vars
  int leftMotorSpeed = 0;
  int rightMotorSpeed = 0;

  int avgMotorSpeed = 0;

  // User control code here, inside the loop 
  while (true) {

    //initializing motorspeed variables
    leftMotorSpeed = Controller1.Axis3.position(percent);
    rightMotorSpeed = Controller1.Axis2.position(percent);
    //if the absolute difference between the sticks is within 10, and they have the same sign, and neither of them are zero
    if((abs(leftMotorSpeed - rightMotorSpeed) <= CONTROLLER_MATCHING_THRESHOLD) && rightMotorSpeed != 0 && leftMotorSpeed != 0){
      //set the motor outputs to the avg of the two
      avgMotorSpeed = ((leftMotorSpeed + rightMotorSpeed) / 2);
      leftMotorSpeed = avgMotorSpeed;
      rightMotorSpeed = avgMotorSpeed;
    }


    if(Controller1.ButtonR1.pressing() || Controller1.ButtonR2.pressing()){
      FlywheelMotorLeft.setVelocity(FLYWHEEL_SPEED, percent);
      FlywheelMotorRight.setVelocity(FLYWHEEL_SPEED, percent);
    }

    else{
      FlywheelMotorLeft.setVelocity(0, percent);
      FlywheelMotorRight.setVelocity(0, percent);
    }

    if(Controller1.ButtonL1.pressing()){
      IntakeMotor.setVelocity(INTAKE_SPEED, percent);
    }
    else if(Controller1.ButtonL2.pressing()){
      IntakeMotor.setVelocity(-1 * INTAKE_SPEED, percent);
    }
    else{
      IntakeMotor.setVelocity(0, percent);
    }

    FlywheelMotorLeft.spin(vex::forward);
    FlywheelMotorRight.spin(vex::forward);

    if(Controller1.ButtonL1.pressing()){
      IntakeMotor.setVelocity(INTAKE_SPEED, percent);
    }
    else if(Controller1.ButtonL2.pressing()){
      IntakeMotor.setVelocity(-1 * INTAKE_SPEED, percent);
    }
    else{
      IntakeMotor.setVelocity(0, percent);
    }

    IntakeMotor.spin(vex::forward);

    if(Controller1.ButtonLeft.pressing() && !Controller1.ButtonRight.pressing()){
      leftMotorSpeed = -FINE_TUNING_SPEED;
      rightMotorSpeed = FINE_TUNING_SPEED;
    }
    else if(!Controller1.ButtonLeft.pressing() && Controller1.ButtonRight.pressing()){
      leftMotorSpeed = FINE_TUNING_SPEED;
      rightMotorSpeed = -FINE_TUNING_SPEED;
    }
    else if(!Controller1.ButtonUp.pressing() && Controller1.ButtonDown.pressing()){
      leftMotorSpeed = -FINE_TUNING_SPEED + FWD_BWD_ADD;
      rightMotorSpeed = -FINE_TUNING_SPEED + FWD_BWD_ADD;
    }
    else if(Controller1.ButtonUp.pressing() && !Controller1.ButtonDown.pressing()){
      leftMotorSpeed = FINE_TUNING_SPEED + FWD_BWD_ADD;
      rightMotorSpeed = FINE_TUNING_SPEED + FWD_BWD_ADD;
    }
    else{
      leftMotorSpeed = controllerCurve(leftMotorSpeed, CURVE);
      rightMotorSpeed = controllerCurve(rightMotorSpeed, CURVE);
    }

    //LeftMotor: Left Stick with deadzone
    if(abs(leftMotorSpeed) <= DEADZONE && abs(rightMotorSpeed) <= DEADZONE) {
      //stopping if joystick within deadzone
      LeftBackMotor.setVelocity(0, percent);
      LeftFrontMotor.setVelocity(0, percent);
      RightBackMotor.setVelocity(0, percent);
      RightFrontMotor.setVelocity(0, percent);
      brakingTimeRemaining -= 25;
    }
    else{
      //setting motor velocity
      LeftBackMotor.setVelocity(leftMotorSpeed, percent);
      LeftFrontMotor.setVelocity(leftMotorSpeed, percent);
      RightBackMotor.setVelocity(rightMotorSpeed, percent);
      RightFrontMotor.setVelocity(rightMotorSpeed, percent);
      brakingTimeRemaining = BRAKING_TIME;
    }

    if(brakingTimeRemaining <= 0){
        LeftBackMotor.stop(hold);
        LeftFrontMotor.stop(hold);
        RightBackMotor.stop(hold);
        RightFrontMotor.stop(hold);
    }
    else{
      //spinning motors and activating hydraulics
      LeftBackMotor.spin(fwd);
      LeftFrontMotor.spin(fwd);
      RightBackMotor.spin(fwd);
      RightFrontMotor.spin(fwd);
    }
    //decrementing braking time so that brakes engage on time
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
}


