#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "pid.cpp"

class Zeroing { //store value of sensor
  public:
    Zeroing(int pin, PID motor1, PID motor2, PID motor3) {
        this->readPin = pin;
        this->motor1 = motor1;
        this->motor2 = motor2;
        this->motor3 = motor3;
    }
    void Setup() {
        stdio_init_all();
        gpio_init(this->readPin);
        gpio_set_dir(this->readPin, GPIO_IN);
    }
    int Read() {
        this->zerod = gpio_get(this->readPin);
        return this->zerod;
    }

  private:
    void zero() { //put in here for now
        while (!(this->Read())) {
            this->motor1.update(1);
            this->motor2.update(1);
            this->motor3.update(1);
        }
    }
    int readPin;
    int zerod = false;
    PID motor1;
    PID motor2;
    PID motor3;
};