#include "control-library.h"

bool calculateCallback(repeating_timer_t *rt);
bool updateIntergralDerivativeCallback(repeating_timer_t *rt);

PID::PID(float P, float I, float D, Motor motor, char tuningMode = PID_TUNING_MODE_POSITION, int timestep = 100, int substep = 10)
{
    this->P = P;
    this->I = I;
    this->D = D;
    this->motor = motor;
    this->timestep = timestep;                       // timestep for updating output
    this->substep = this->timestep / (float)substep; // how many times to update integral/derivative per timestep
    this->tuningMode = tuningMode;
}

void PID::Setup()
{ // PICO SPECIFIC: create repeating timer, setup callbacks
    repeating_timer_t timer;
    this->timer = timer;
    add_repeating_timer_ms(this->timestep, calculateCallback, this, &timer);
    add_repeating_timer_ms(this->substep, updateIntergralDerivativeCallback, this, &timer);
    this->motor.Setup();
    this->active = true;
}

void PID::updateTarget(float target)
{
    // Idea; pid loop is autonomous and will automatically do whatever due to repeating timers
    // if we just update the target with a function call we might be okay
    this->target = target;
}

void PID::reset()
{
    this->output = 0;
    this->target = 0;
    this->lastError = 0;
}

/***
 * setActive: sets flag to toggle function of pid callbacks
 */
void PID::setActive(bool active)
{
    this->active = active;
}

void PID::calculate()
{
    if (this->active)
    {
        this->output = this->P * this->target + this->I * this->Integral + this->D * this->Derivative;
        this->motor.SetPwmPercentage(output);
    }
}

void PID::updateErrorIntegralDerivative()
{
    if (this->active)
    {
        float error; // error term
        if (this->tuningMode = PID_TUNING_MODE_POSITION)
        {
            error = this->motor.GetPosition() - this->target;
        }
        else
        {
            error = this->motor.GetVelocity() - this->target;
        }

        // substep is in miliseconds
        this->Integral += error * (this->substep / MS_IN_SEC);                      // left riemann sum at fixed substep
        this->Derivative = (error - this->lastError) / (this->substep / MS_IN_SEC); // estimate of derivatvie at fixed substep
        this->lastError = error;
    }
}

bool calculateCallback(repeating_timer_t *rt)
{
    ((PID *)rt->user_data)->calculate();
    return true;
};

bool updateIntergralDerivativeCallback(repeating_timer_t *rt)
{
    ((PID *)rt->user_data)->updateErrorIntegralDerivative();
    return true;
};

SwerveDrive::SwerveDrive(PID turnPID, PID drivePID)
{
    this->turnPID = turnPID;
    this->drivePID = drivePID;
}

void SwerveDrive::Setup()
{
    this->turnPID.Setup();
    this->drivePID.Setup();
}

void SwerveDrive::SetPidTargets(float turnMotorPercentage, float wheelMotorPercentage)
{
    this->turnPID.updateTarget(turnMotorPercentage);
    this->drivePID.updateTarget(wheelMotorPercentage);
}

void SwerveDrive::TogglePids(bool value)
{
    this->turnPID.setActive(value);
    this->drivePID.setActive(value);
}

void SwerveDrive::ResetPids()
{
    this->turnPID.reset();
    this->drivePID.reset();
}