#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "quadrature_encoder.cpp"

void calculate();
void updateErrorIntegralDerivative();
class pid { //pid loop
  public:
    pid(float P, float I, float D, Encoder encoder, int timestep = 100, int substep = 10) {
        this->P = P;
        this->I = I;
        this->D = D;
        this->encoder = encoder;
        this->timestep = timestep //timestep for updating output
        this->substep = this->timestep / (float)substep // how many times to update integral/derivative per timestep
        
    }

    void setup(){ // PICO SPECIFIC: create repeating timer, setup callbacks
        this->timer = repeating_timer_t timer;
        add_repeating_timer_ms(this->timestep, calculate, NULL, timer);
        add_repeating_timer_ms(10, updateErrorIntegralDerivative, NULL, timer);
    }

    void updateTarget(float target){
      //Idea; pid loop is autonomous and will automatically do whatever due to repeating timers
      //if we just update the target with a function call we might be okay
      this->target = target;
    }

    float output(){
      return this->output;
    }
  private:
    //privated functions for pid function
    void calculate(){
        this->output = this->P * this->target + this->I * this->Integral + this->D * this->Derivative;
    }

    void updateErrorIntegralDerivative(){
      error = this->encoder.getPosition()-this->target; //error term
      this->Integral += error * this->substep; //left riemann sum at fixed substep
      this->Derivative = (this->encoder.getPosition()-this->output)/2; //estimate of derivatvie at fixed substep
    }

    float P;
    float I;
    float D;

    float target = 0;
    float output = 0;
    float input = 0;

    float Integral = 0;
    float Derivative = 0;
    repeating_timer_t timer = {};
    int timestep;
    int substep;
    Encoder encoder;
};