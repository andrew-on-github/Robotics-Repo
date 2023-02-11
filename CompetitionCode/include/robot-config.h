using namespace vex;

extern brain Brain;

// VEXcode devices
extern controller Controller1;
extern limit MenuCycle;
extern limit MenuSelect;
extern motor LeftFrontMotor;
extern motor LeftBackMotor;
extern motor RightFrontMotor;
extern motor RightBackMotor;
extern motor FlywheelMotorLeft;
extern motor FlywheelMotorRight;
extern motor IntakeMotor;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );