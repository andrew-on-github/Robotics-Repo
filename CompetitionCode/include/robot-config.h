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
extern motor LiftMotor;
extern motor MobileGoalMotor;
extern digital_in AutoTest;
extern motor ClampMotor;
extern potV2 LiftPot;
extern potV2 MobileGoalPot;
extern pot ClampPot;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );