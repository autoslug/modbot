#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

class pid { //pid loop
  public:
    pid() {
        //init pid constants
    }
    float calculate(float input, float timeStep){
        float output = this->P * input + this-> I * this -> Integral;
        output += this-> D * LastOut - output / timeStep;
        Integral+=(output*timeStep)
        return output
    }

  private:
    bool callback(repeating_timer_t *rt){
        //calculate
    }
    void loop(){
        repeating_timer_t timer; // (add to main, look at periodic sampler)
        add_repeating_timer_ms(100, callback, NULL, timer);
    }
    float P = 1;
    float I = 1;
    float D = 1;
    float Integral = 0;
    float LastOut = 0;
};