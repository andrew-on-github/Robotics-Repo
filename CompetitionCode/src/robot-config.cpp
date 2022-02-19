#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
controller Controller1 = controller(primary);
limit MenuCycle = limit(Brain.ThreeWirePort.A);
limit MenuSelect = limit(Brain.ThreeWirePort.B);
motor LeftFrontMotor = motor(PORT15, ratio18_1, false);
motor LeftBackMotor = motor(PORT8, ratio18_1, false);
motor RightFrontMotor = motor(PORT12, ratio18_1, true);
motor RightBackMotor = motor(PORT7, ratio18_1, true);
motor IntakeMotor = motor(PORT18, ratio18_1, true);
motor MobileGoalMotor = motor(PORT13, ratio18_1, false);
digital_in AutoTest = digital_in(Brain.ThreeWirePort.C);
potV2 MobileGoalPot = potV2(Brain.ThreeWirePort.D);
potV2 LiftPot = potV2(Brain.ThreeWirePort.E);
potV2 IVAAD = potV2(Brain.ThreeWirePort.H);
pneumatics ClampPiston = pneumatics(Brain.ThreeWirePort.F);
motor LiftMotorMotorA = motor(PORT17, ratio36_1, true);
motor LiftMotorMotorB = motor(PORT14, ratio36_1, false);
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