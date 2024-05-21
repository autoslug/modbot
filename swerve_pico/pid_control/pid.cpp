#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "quadrature_encoder.cpp"

void calculate();
void updateErrorIntegralDerivative();
class PID { //pid loop
  public:
    PID(float P, float I, float D, Encoder encoder, int timestep = 100, int substep = 10) {
        this->P = P;
        this->I = I;
        this->D = D;
        this->encoder = encoder;
        this->timestep = timestep //timestep for updating output
        this->substep = this->timestep / (float)substep // how many times to update integral/derivative per timestep
        this->swerveModule = swerveModule;
    }

    void setup(){ // PICO SPECIFIC: create repeating timer, setup callbacks
        this->timer = repeating_timer_t timer;
        add_repeating_timer_ms(this->timestep, calculate, NULL, timer);
        add_repeating_timer_ms(10, updateErrorIntegralDerivative, NULL, timer); // TODO: why is it 10?
    }

    void updateTarget(float target){
      //Idea; pid loop is autonomous and will automatically do whatever due to repeating timers
      //if we just update the target with a function call we might be okay
      this->target = target;
    }

    // float getOutput(){ //random messy function for debugging? :( (TLDR: WHY IS THIS HERE IM JUST GONNA KEEP IT HERE ;-;)
    //   return this->output;
    // }
  private:
    //privated functions for pid function
    void calculate(){
        this->output = this->P * this->target + this->I * this->Integral + this->D * this->Derivative;
    }

    void updateErrorIntegralDerivative(){
      error = this->encoder.getPosition()-this->target; //error term
      this->Integral += error * (this->substep / 1000); //left riemann sum at fixed substep
      this->Derivative = (error-this->lastError)/(this->substep / 1000); //estimate of derivatvie at fixed substep
      this->lastError = error;
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
    Encoder encoder;
};