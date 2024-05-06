#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

float calculate(float input, float timeStep);
void updateIntegralDerivative();
class pid { //pid loop
  public:
    pid(float P, float I, float D) {
        this->P = P;
        this->I = I;
        this->D = D;
    }

    setup(){ // PICO SPECIFIC: create repeating timer, setup callbacks
        this->timer = repeating_timer_t timer;
        add_repeating_timer_ms(100, calculate, NULL, timer);
        add_repeating_timer_ms(10, updateIntegralDerivative, NULL, timer);
    }

  private:
    
    void calculate(float input, float timeStep){
        this->output = this->P * input + this-> I * this -> Integral + this-> D * this -> Derivative;
    }

    float P;
    float I;
    float D;
    repeating_timer_t timer = {};
    float Integral = 0;
    float Derivative = 0;
    float output = 0;
};