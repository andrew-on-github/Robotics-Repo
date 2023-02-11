using namespace vex;

extern brain Brain;

// VEXcode devices
extern controller Controller1;
extern limit MenuCycle;
extern limit MenuSelect;
extern motor LeftBackMotor;
extern motor RightBackMotor;
extern digital_out GreenLight;
extern digital_out YellowLight;
extern digital_out RedLight;
extern motor IntakeMotor;
extern motor FlywheelMotor;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );