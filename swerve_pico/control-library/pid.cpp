#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "motor-library.cpp"

#define MS_IN_SEC 1000 //DOUBLE CHECK VALUE OF CONVERSION

void calculate();
void updateErrorIntegralDerivative();

enum PIDTuningMode{
  PID_TUNING_MODE_POSITION,
  PID_TUNING_MODE_VELOCITY
};

class PID { //pid loop
  public:
    PID(float P, float I, float D, Motor motor, char tuningMode = PID_TUNING_MODE_POSITION, int timestep = 100, int substep = 10) {
        this->P = P;
        this->I = I;
        this->D = D;
        this->motor = motor;
        this->timestep = timestep //timestep for updating output
        this->substep = this->timestep / (float)substep // how many times to update integral/derivative per timestep
        this->tuningMode = tuningMode;
    }

    void setup(){ // PICO SPECIFIC: create repeating timer, setup callbacks
        this->timer = repeating_timer_t timer;
        add_repeating_timer_ms(this->timestep, calculate, NULL, timer);
        add_repeating_timer_ms(this->substep, updateErrorIntegralDerivative, NULL, timer);
        this->motor.Setup();
        this->active = true;
    }

    void updateTarget(float target){
      //Idea; pid loop is autonomous and will automatically do whatever due to repeating timers
      //if we just update the target with a function call we might be okay
      this->target = target;
    }

    void reset(){
      this->output = 0;
      this->target = 0;
      this->lastError = 0;      
    }

    /***
     * setActive: sets flag to toggle function of pid callbacks
     */
    void setActive(bool active){
      this->active = active;
    }
  private:
    //privated functions for pid function
    void calculate(){
        if(this->active){
          this->output = this->P * this->target + this->I * this->Integral + this->D * this->Derivative;
          this->motor.SetPwmPercentage(output);
        }
    }

    void updateErrorIntegralDerivative(){
      if(this->active){
        float error; //error term
        if(this->tuningMode = PID_TUNING_MODE_POSITION){
          error = this->motor.getPosition() - this->target;
        }else{
          error = this->motor.GetVelocity() - this->target;
        }
        
        //substep is in miliseconds
        this->Integral += error * (this->substep / MS_IN_SEC); //left riemann sum at fixed substep
        this->Derivative = (error - this->lastError) / (this->substep / MS_IN_SEC); //estimate of derivatvie at fixed substep
        this->lastError = error;
      }
    }

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