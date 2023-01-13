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
motor LeftBackMotor = motor(PORT10, ratio18_1, false);
motor RightBackMotor = motor(PORT8, ratio18_1, true);
digital_out GreenLight = digital_out(Brain.ThreeWirePort.A);
digital_out YellowLight = digital_out(Brain.ThreeWirePort.B);
digital_out RedLight = digital_out(Brain.ThreeWirePort.C);
motor FlywheelMotorsMotorA = motor(PORT20, ratio6_1, false);
motor FlywheelMotorsMotorB = motor(PORT21, ratio6_1, true);
motor_group FlywheelMotors = motor_group(FlywheelMotorsMotorA, FlywheelMotorsMotorB);
motor IntakeMotor = motor(PORT16, ratio18_1, false);

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