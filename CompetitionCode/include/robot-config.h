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
extern digital_out ClampPiston;
extern motor IntakeMotor;
extern motor LiftMotor;
extern motor MobileGoalMotor;
extern digital_in TestJump;
extern digital_in AutoTest;
extern limit MobileGoalSwitch;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );