#include <iostream>
#include <pico/stdlib.h>

#include "motor-library.h"

#define MS_IN_SEC 1000 // DOUBLE CHECK VALUE OF CONVERSION

enum PIDTuningMode
{
  PID_TUNING_MODE_POSITION,
  PID_TUNING_MODE_VELOCITY
};


class PID
{ // pid loop
public:
  PID();
  PID(float P, float I, float D, Motor motor, char tuningMode = PID_TUNING_MODE_POSITION, int timestep = 100, int substep = 10);
  void Setup();
  void updateTarget(float target);
  void reset();
  void setActive(bool active);

  void calculate();
  void updateErrorIntegralDerivative();
private:

  float P;
  float I;
  float D;

  float target = 0;
  float output = 0;
  float input = 0;
  float lastError = 0;

  float Integral = 0;
  float Derivative = 0;
  repeating_timer_t timer = {};
  int timestep;
  int substep;
  Motor motor;
  char tuningMode;
  bool active = false;
};

/**
 * Class to hold two pids, represents one swerve module (one turn wheel and one drive wheel)
 */
class SwerveDrive
{
public:
  SwerveDrive(PID turnPID, PID drivePID);
  void Setup();
  void SetPidTargets(float turnMotorPercentage, float wheelMotorPercentage);
  void TogglePids(bool value);
  void ResetPids();

private:
  PID turnPID;
  PID drivePID;
};
