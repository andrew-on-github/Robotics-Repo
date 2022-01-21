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
motor LeftFrontMotor = motor(PORT1, ratio18_1, false);
motor LeftBackMotor = motor(PORT2, ratio18_1, false);
motor RightFrontMotor = motor(PORT9, ratio18_1, false);
motor RightBackMotor = motor(PORT10, ratio18_1, false);

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