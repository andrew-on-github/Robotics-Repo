#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
controller Controller1 = controller(primary);
limit MenuCycle = limit(Brain.ThreeWirePort.D);
limit MenuSelect = limit(Brain.ThreeWirePort.E);
motor LeftFrontMotor = motor(PORT2, ratio18_1, false);
motor LeftBackMotor = motor(PORT11, ratio18_1, false);
motor RightFrontMotor = motor(PORT8, ratio18_1, true);
motor RightBackMotor = motor(PORT18, ratio18_1, true);
motor MobileGoalMotor = motor(PORT3, ratio18_1, false);
digital_in AutoTest = digital_in(Brain.ThreeWirePort.H);
motor ClampMotor = motor(PORT17, ratio18_1, true);
potV2 LiftPot = potV2(Brain.ThreeWirePort.A);
potV2 MobileGoalPot = potV2(Brain.ThreeWirePort.B);
pot ClampPot = pot(Brain.ThreeWirePort.C);
motor LiftMotorMotorA = motor(PORT19, ratio36_1, true);
motor LiftMotorMotorB = motor(PORT20, ratio36_1, false);
motor_group LiftMotor = motor_group(LiftMotorMotorA, LiftMotorMotorB);

// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}