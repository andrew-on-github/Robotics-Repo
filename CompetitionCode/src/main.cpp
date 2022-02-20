//git julian
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
// LeftFrontMotor       motor         15              
// LeftBackMotor        motor         8               
// RightFrontMotor      motor         12              
// RightBackMotor       motor         7               
// IntakeMotor          motor         18              
// LiftMotor            motor         4               
// MobileGoalMotor      motor         13              
// AutoTest             digital_in    C               
// MobileGoalPot        potV2         D               
// LiftPot              potV2         E               
// IVAAD                potV2         H               
// RightClampPiston     pneumatics    G   
// LeftClampPiston      pneumatics    F            
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

#include "motor-controller.h"

using namespace vex;
using namespace std;

// A global instance of competition
competition Competition;

//global motorgroup
motor_group LeftMotorGroup(LeftFrontMotor, LeftBackMotor);
motor_group RightMotorGroup(RightFrontMotor, RightBackMotor);

motor_group DriveMotorGroup(LeftFrontMotor, LeftBackMotor, RightFrontMotor, RightBackMotor);

//motor controller objects 
MotorController* LiftMotorController;
MotorController* MobileGoalMotorController;

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

//global to count the number of actuations of clamp
int clampActuations = 0;

// target angle of the lift
const double LIFT_HIGH_POSITION = 14;
const double LIFT_LOW_POSITION = 102;
const double LIFT_MID_POSITION = 45;

const double LIFT_TAU = 0.25;
const double MOBILE_GOAL_TAU = 0.25;

double liftTarget = LIFT_LOW_POSITION;


const double EPSILON = 1E-5;

// target angle of the mobile goal
const double MOBILE_GOAL_EXTENDED = 216;
const double MOBILE_GOAL_RETRACTED = 132;
double mobileGoalTarget = MOBILE_GOAL_RETRACTED;

//declaring and initializing preauto flag
bool preauto = true;

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

//true = right arrow high toggle, false = l2 low toggle
void liftFSA(bool isHighToggle){
  if(isHighToggle && fabs(liftTarget - LIFT_LOW_POSITION) < EPSILON) {
    liftTarget = LIFT_HIGH_POSITION;
    LiftMotorController-> setMaxSpeedThresh(MC_THRESHOLD_LIFT_UP);
  }
  else if(isHighToggle && fabs(liftTarget - LIFT_HIGH_POSITION) < EPSILON) {
    liftTarget = LIFT_MID_POSITION;
    LiftMotorController-> setMaxSpeedThresh(MC_THRESHOLD_LIFT_DOWN);
  }
  else if(isHighToggle && fabs(liftTarget - LIFT_MID_POSITION) < EPSILON) {
    liftTarget = LIFT_HIGH_POSITION;
    LiftMotorController-> setMaxSpeedThresh(MC_THRESHOLD_LIFT_UP);
  }
  else if(!isHighToggle && fabs(liftTarget - LIFT_MID_POSITION) < EPSILON) {
    liftTarget = LIFT_LOW_POSITION;
    LiftMotorController-> setMaxSpeedThresh(MC_THRESHOLD_LIFT_DOWN);
  }
  else if(!isHighToggle && fabs(liftTarget - LIFT_HIGH_POSITION) < EPSILON) {
    liftTarget = LIFT_LOW_POSITION;
    LiftMotorController-> setMaxSpeedThresh(MC_THRESHOLD_LIFT_DOWN);
  }
  else if(!isHighToggle && fabs(liftTarget - LIFT_LOW_POSITION) < EPSILON) {
    liftTarget = LIFT_MID_POSITION;
    LiftMotorController-> setMaxSpeedThresh(MC_THRESHOLD_LIFT_UP);
  }
}

/*void lowerLift(){
  LiftMotor.setVelocity(-100, percent);
  LiftMotor.spin(fwd);
  while(LiftPot.angle(degrees) > 45 ){
    wait(25, msec);
  }
  LiftMotor.stop();
}*/

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
  
  motor motors[5] = {LeftFrontMotor, LeftBackMotor, RightFrontMotor, RightBackMotor, MobileGoalMotor};

  int hiMotor = 0;
  int warningTemp = 100; //temperature at which the brain throttles control

  Brain.Timer.reset();

  while(true){
    //timer calculations

    //subtracting seconds since brain timer reset from 105 (user control time in seconds)
    totalSecondsRemaining = 105 - ((int) Brain.Timer.time(seconds));
    if(totalSecondsRemaining < 0){
      totalSecondsRemaining = 0;
    } 

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
      Controller1.Screen.newLine();
      Controller1.Screen.print("LIFT ANGLE: %.2f", LiftPot.angle(degrees));
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

  //set brain to user double buffered input
  Brain.Screen.render();
  
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

  //preauto flag turns false when usercontrol or autonomous begins
  while(preauto){
    //if menucycle is pressed and auton is not locked in
    if((MenuCycle.pressing() && !menuCycleLast) && !selected){
      //increment auton, rollover to 0
      if(selectedAuto > 2){
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

    //wait statement
    wait(25, msec);

    //clearing screen to make room for next cycle
    Brain.Screen.clearScreen();

    //setting last cycle variables
    menuCycleLast = MenuCycle.pressing();
    menuSelectLast = MenuSelect.pressing();
  }
  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

void setVelocityAllMotors(double val) {
  LeftFrontMotor.setVelocity(val, percent);
  LeftBackMotor.setVelocity(val, percent);
  RightFrontMotor.setVelocity(val, percent);
  RightBackMotor.setVelocity(val, percent);
}
void setDirectionAllMotors(directionType dir) {
  LeftFrontMotor.spin(dir);
  RightFrontMotor.spin(dir);
  LeftBackMotor.spin(dir);
  RightBackMotor.spin(dir);
}
void stopAllMotors() {
  LeftFrontMotor.stop(coast);
  LeftBackMotor.stop(coast);
  RightFrontMotor.stop(coast);
  RightBackMotor.stop(coast);
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

  Brain.Screen.print("Robot under autonomous control. Please stand clear.");
  Brain.Screen.newLine();
  Controller1.Screen.print("AUTO");

    switch(selectedAuto){
      case 0:
        Brain.Screen.print("0");
        Controller1.Screen.print("AUTO 0");

        // turn off hold
        DriveMotorGroup.stop();
        wait(100, msec); // .1 s

        // lower lift
        mobileGoalFSA();
        wait(800, msec); // .6 s
       

        // drive forward to first goalpost
        DriveMotorGroup.setVelocity(100, percent);
        DriveMotorGroup.spin(reverse);
        liftFSA(true);
        wait(400, msec); // 1.45 s
        DriveMotorGroup.setVelocity(0, percent);
        DriveMotorGroup.stop();
        wait(100, msec); // 1.55 s

        // lift goalpost
        mobileGoalFSA();
        wait(1200,msec); // 2.75 s
        //MobileGoalMotor.stop();
        //wait(100,msec); // 2.85 s


        /*setVelocityAllMotors(100);
        LeftFrontMotor.spinFor(reverse, 1000, msec);
        LeftBackMotor.spinFor(reverse,1000, msec);
        RightFrontMotor.spinFor(fwd,1000, msec);
        RightBackMotor.spinFor(fwd,1000,msec);
        setVelocityAllMotors(100);
        setDirectionAllMotors(fwd);
        wait(100,msec);
        setVelocityAllMotors(0);
        stopAllMotors(); */


        // run intake
        IntakeMotor.setVelocity(100,percent);
        IntakeMotor.spin(fwd);
        wait(3000,msec); // 5.85 s
        IntakeMotor.stop();
        DriveMotorGroup.setVelocity(50, percent);
        DriveMotorGroup.spin(reverse);
        IntakeMotor.setVelocity(100,percent);
        IntakeMotor.spin(fwd);
        wait(100,msec);
        IntakeMotor.stop();
        DriveMotorGroup.setVelocity(50,percent);
        DriveMotorGroup.spin(fwd);
        IntakeMotor.setVelocity(100,percent);
        IntakeMotor.spin(fwd);
        wait(100,msec);
        IntakeMotor.stop();
        
        

        // spin 180
        /*setVelocityAllMotors(80);
        LeftFrontMotor.spinFor(reverse, 1600, msec);
        LeftBackMotor.spinFor(reverse,1600, msec);
        RightFrontMotor.spinFor(fwd,1600, msec);
        RightBackMotor.spinFor(fwd,1600,msec);
        stopAllMotors();
        wait(100, msec); */

        // put goalpost down
        wait(100,msec);
        DriveMotorGroup.stop(hold);
        mobileGoalFSA();
        wait(800, msec);

        // back up
        liftFSA(false);
        DriveMotorGroup.setVelocity(-50,percent);
        DriveMotorGroup.spin(reverse);
        wait(800,msec);
        DriveMotorGroup.stop();

        
        break;
      
      case 1:
        Brain.Screen.print("1");
        Controller1.Screen.print("AUTO 1");
        
        //Stop hold on DT
        DriveMotorGroup.stop();
        wait(100, msec);

        liftFSA(true);


        break;

      case 2:
        Brain.Screen.print("2");
        Controller1.Screen.print("AUTO 2");
        break;

      case 3:
        Brain.Screen.print("3");
        Controller1.Screen.print("AUTO 3");
        break;

      default:
        Brain.Screen.print("error");
        Controller1.Screen.print("error");
        break;
    }
    //Brain.Screen.clearScreen();
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

  //resetting clampactuations
  clampActuations = 0;

  //updating flag to cause preauton method to exit
  preauto = false;

  //allowing motor controllers to control their motors
  LiftMotorController->setEnabled(true);
  MobileGoalMotorController->setEnabled(true);

  //clearing screen of anything printed in pre-auto
  Brain.Screen.clearScreen();

  //declaring and initializing clamp variables
  bool clamp = false;
  bool clampLast = false;

  //declaring and initializing mobile goal variables
  bool l1Last = false;

  //last value of the l2 button
  bool l2Last = false;

  //last value of the y button
  bool yLast = false;

  //last value of the right button
  bool rightLast = false;

  //mobile goal control var
  //bool mobileGoalFwd = false;

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
      LeftBackMotor.stop();
      LeftFrontMotor.stop();
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

    //Lift: L2 Toggles between Low and target positions
    //Right toggles target between the highest value and the target value
    if(Controller1.ButtonL2.pressing() && !l2Last){
      liftFSA(false);
    }
    else if(Controller1.ButtonRight.pressing() && !rightLast){
      liftFSA(true);
    }

    //Intake: R1 Fwd Y rev
    if(Controller1.ButtonR1.pressing() && fabs(liftTarget - LIFT_LOW_POSITION) > EPSILON){
      IntakeMotor.setVelocity(100, percent);
    }
    else if(Controller1.ButtonY.pressing() && fabs(liftTarget - LIFT_LOW_POSITION) > EPSILON){
      IntakeMotor.setVelocity(-100, percent);
    }
    else{
      IntakeMotor.setVelocity(0, percent);
    }

    //Clamp: R2 toggles
    if(Controller1.ButtonR2.pressing() && !clampLast){
      clamp = !clamp;
      if(!clamp){
        clampActuations++;
      }
    }

    //MobileGoalMotor: L1 toggles
    if(Controller1.ButtonL1.pressing() && !l1Last){
      mobileGoalFSA();
    }


    
    //spinning motors and activating hydraulics
    LeftBackMotor.spin(fwd);
    LeftFrontMotor.spin(fwd);
    RightBackMotor.spin(fwd);
    RightFrontMotor.spin(fwd);

    IntakeMotor.spin(fwd);

    ClampPiston.set(clamp);

    //update clamplast so inputs arent counted multiple times
    clampLast = Controller1.ButtonR2.pressing();
    l1Last = Controller1.ButtonL1.pressing();
    l2Last = Controller1.ButtonL2.pressing();
    yLast = Controller1.ButtonY.pressing();
    rightLast = Controller1.ButtonRight.pressing();
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
  /*while (true) {
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
  }*/
}