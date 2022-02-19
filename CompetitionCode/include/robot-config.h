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
extern motor IntakeMotor;
extern motor LiftMotorA;
extern motor LiftMotorB;
extern motor_group LiftMotor;
extern motor MobileGoalMotor;
extern digital_in AutoTest;
extern potV2 MobileGoalPot;
extern potV2 LiftPot;
extern potV2 IVAAD;
extern pneumatics ClampPiston;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );