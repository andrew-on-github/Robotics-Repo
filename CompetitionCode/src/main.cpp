//git main
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
// MenuCycle            limit         D               
// MenuSelect           limit         E               
// LeftFrontMotor       motor         2               
// LeftBackMotor        motor         11              
// RightFrontMotor      motor         8               
// RightBackMotor       motor         18              
// MobileGoalMotor      motor         3               
// AutoTest             digital_in    H               
// ClampMotor           motor         17              
// LiftPot              potV2         A               
// MobileGoalPot        potV2         B               
// ClampPot             pot           C               
// LeftLiftMotor        motor         19              
// RightLiftMotor       motor         20              
// InertialSensor       inertial      15              
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

#include "motor-controller.h"
#include "position.h"

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


//motor controller objects 
MotorController* LiftMotorController;
MotorController* MobileGoalMotorController;
MotorController* ClampMotorController;

PositionMonitor* RobotPosition;

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

//global to count the number of actuations of clamp
int clampActuations = 0;

//const to decide the amount of time before braking starts. 1 braking time = 1 usercontrol 
//loop cycle = .25 msec
const int BRAKING_TIME = 500;

//var to count amount of time reamining before breaking
int brakingTimeReamining = BRAKING_TIME;

//wheel circumfrence const
const double WHEEL_CIRCUMFRENCE = 12.56637;

//delta time constant
const double DELTA_TIME = 10;

//amount of time in teh user control portion of the match in seconds
const int USERCONTROL_TIME_SECONDS = 105;

// target angle of the lift
const double LIFT_HIGH_POSITION = 120;
const double LIFT_LOW_POSITION = 27;

const double CLAMP_OUT_POSITION = 187;
const double CLAMP_IN_POSITION = 80;

const double LIFT_TAU = 0.25;
const double MOBILE_GOAL_TAU = 0.25;
const double CLAMP_TAU = 2.5;

double liftTarget = LIFT_LOW_POSITION;
double clampTarget = CLAMP_IN_POSITION;

const double EPSILON = 1E-5;

// target angle of the mobile goal
const double MOBILE_GOAL_EXTENDED = 190;
const double MOBILE_GOAL_RETRACTED = 125;
double mobileGoalTarget = MOBILE_GOAL_RETRACTED;

//declaring and initializing preauto flag
bool preauto = true;

//declaring and initializing liftmotor group
motor_group LiftMotor = motor_group(LeftLiftMotor, RightLiftMotor);

void mobileGoalFSA(){
 if(fabs(mobileGoalTarget - MOBILE_GOAL_EXTENDED) < EPSILON) {
   mobileGoalTarget = MOBILE_GOAL_RETRACTED;
   MobileGoalMotorController->setOverRidePercent(-100);
   MobileGoalMotorController->setOverRideCycles(15);
 }
 else{
   mobileGoalTarget = MOBILE_GOAL_EXTENDED;
 }
}

const double MC_THRESHOLD_LIFT_UP = 10;
const double MC_THRESHOLD_LIFT_DOWN = 1000000;

void clampFSA(){
  if(fabs(clampTarget - CLAMP_OUT_POSITION) < EPSILON){
    clampTarget = CLAMP_IN_POSITION;
  }
  else{
    clampTarget = CLAMP_OUT_POSITION;
  }
}

//l2
void liftFSA(){
  if(fabs(liftTarget - LIFT_LOW_POSITION) < EPSILON) {
    liftTarget = LIFT_HIGH_POSITION;
    LiftMotorController-> setMaxSpeedThresh(MC_THRESHOLD_LIFT_UP);
  }
  else if(fabs(liftTarget - LIFT_HIGH_POSITION) < EPSILON) {
    liftTarget = LIFT_LOW_POSITION;
    LiftMotorController-> setMaxSpeedThresh(MC_THRESHOLD_LIFT_DOWN);
  }
}

int controllerCurve(int input, double curve){
  
  double dubInput = input;

  dubInput /= 100;

  dubInput = pow(dubInput, curve);

  dubInput *= 100;

  if(input >= 0){
    return (int)dubInput;
  }
  return -((int)(fabs(dubInput)));

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
  motor* motors[7] = {&LeftFrontMotor, &LeftBackMotor, &RightFrontMotor, &RightBackMotor, &LeftLiftMotor, &RightLiftMotor, &MobileGoalMotor};

  motor* hiMotor = 0;
  const int WARNING_TEMP = 100; //temperature at which the brain throttles control

  Brain.Timer.reset();

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
      else if(hiMotor == motors[4] || hiMotor == motors[5]){
        Controller1.Screen.print("LM");
      }
      else if(hiMotor == motors[6]){
        Controller1.Screen.print("MG");
      }
      Controller1.Screen.print(" WARN");
      Controller1.Screen.newLine();
      Controller1.Screen.print("%f °C", hiMotor->temperature(celsius));
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
      Controller1.Screen.newLine();
      Controller1.Screen.print("X POS: %.2f", RobotPosition->getXPos());
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

  //initializing motor controllers
  LiftMotorController = new MotorController(&LiftMotor, &LiftPot, &liftTarget, LIFT_TAU);
  MobileGoalMotorController = new MotorController(&MobileGoalMotor, &MobileGoalPot, &mobileGoalTarget, MOBILE_GOAL_TAU);
  ClampMotorController = new MotorController(&ClampMotor, &ClampPot, &clampTarget, CLAMP_TAU);

  //initializing position monitor
  RobotPosition = new PositionMonitor(&InertialSensor, DELTA_TIME, msec);

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

  LiftMotorController->setEnabled(true);
  MobileGoalMotorController->setEnabled(true);
  ClampMotorController->setEnabled(true);

  Brain.Screen.print("Robot under autonomous control. Please stand clear.");
  Controller1.Screen.print("AUTO");

    switch(selectedAuto){
      case 0:
        //wheel circumfrence = 12.56637", divide travel distance in inches by wheel circumfrence constatn"
        mobileGoalFSA();
        DriveMotorGroup.spinFor(vex::forward, 80 / WHEEL_CIRCUMFRENCE, rev, 60 * 30 / WHEEL_CIRCUMFRENCE / .85, rpm, false);
        wait(1.75, sec);
        mobileGoalFSA();
        wait(1, sec);
        DriveMotorGroup.spinFor(reverse, 75 / WHEEL_CIRCUMFRENCE, rev, 60 * 30 / WHEEL_CIRCUMFRENCE, rpm, false);
        wait(1, sec);
        mobileGoalFSA();

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
  LiftMotorController->setEnabled(false);
  MobileGoalMotorController->setEnabled(false);
  ClampMotorController->setEnabled(false);
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

  //allowing motor controllers to control their motors
  LiftMotorController->setEnabled(true);
  MobileGoalMotorController->setEnabled(true);
  ClampMotorController->setEnabled(true);

  //declaring and initializing last vars
  bool l1Last = false;
  bool r2Last = false;
  bool l2Last = false;
  bool rightLast = false;

  //default deadzone value 
  //want this to be as low as possible without any drift
  //test by printing input from the stick when its totally neutral and set this as one above the highest number displayed
  int deadzone = 3;

  //declaring motor speed vars
  int leftMotorSpeed = 0;
  int rightMotorSpeed = 0;

  int avgMotorSpeed = 0;

  // User control code here, inside the loop 
  while (true) {

    //initializing motorspeed variables
    leftMotorSpeed = Controller1.Axis3.position(percent);
    rightMotorSpeed = Controller1.Axis2.position(percent);
    //if the absolute difference between the sticks is within 10, and they have the same sign
    if((abs(leftMotorSpeed - rightMotorSpeed) <= 10)){
      //set the motor outputs to the avg of the two
      avgMotorSpeed = ((leftMotorSpeed + rightMotorSpeed) / 2);
      leftMotorSpeed = avgMotorSpeed;
      rightMotorSpeed = avgMotorSpeed;
    }

    //LeftMotor: Left Stick with deadzone
    if(abs(leftMotorSpeed) < deadzone) {
      //stopping if joystick within deadzone
      LeftBackMotor.setVelocity(0, percent);
      LeftFrontMotor.setVelocity(0, percent);
      //sets motors to brake mode
      if(brakingTimeReamining <= 0){
        LeftBackMotor.stop(hold);
        LeftFrontMotor.stop(hold);
      }
      else if(brakingTimeReamining > 0){
        brakingTimeReamining--;
      }
    }
    else{
      //setting motor velocity
      LeftBackMotor.setVelocity(leftMotorSpeed, percent);
      LeftFrontMotor.setVelocity(leftMotorSpeed, percent);
      brakingTimeReamining = BRAKING_TIME;
    }

    //same as above
    if(abs(rightMotorSpeed) < deadzone) {
      RightBackMotor.setVelocity(0, percent);
      RightFrontMotor.setVelocity(0, percent);
      if(brakingTimeReamining <= 0){
        RightBackMotor.stop(hold);
        RightFrontMotor.stop(hold);
      }
      else if(brakingTimeReamining >0 ){
        brakingTimeReamining--;
      }
    }
    else{
      RightBackMotor.setVelocity(rightMotorSpeed, percent);
      RightFrontMotor.setVelocity(rightMotorSpeed, percent);
      brakingTimeReamining = BRAKING_TIME;
    }

    //Lift: L2 Toggles between Low and target positions
    //Right toggles target between the highest value and the target value
    if(Controller1.ButtonL2.pressing() && !l2Last){
      liftFSA();
    }

    //Clamp: R2 toggles
    if(Controller1.ButtonR2.pressing() && !r2Last){
      clampFSA();
    }

    //Mobile Goal: L1 toggles
    if(Controller1.ButtonL1.pressing() && !l1Last){
      mobileGoalFSA();
    }
    
    //spinning motors and activating hydraulics
    //LeftBackMotor.spin(fwd);
    //LeftFrontMotor.spin(fwd);
    //RightBackMotor.spin(fwd);
    //RightFrontMotor.spin(fwd);
    
    //update clamplast so inputs arent counted multiple times
    r2Last = Controller1.ButtonR2.pressing();
    l2Last = Controller1.ButtonL2.pressing();
    rightLast = Controller1.ButtonRight.pressing();
    l1Last = Controller1.ButtonL1.pressing();

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

