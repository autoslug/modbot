#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "pid.cpp"

class Zeroing { //store value of sensor
  public:
    Zeroing(int pin1, int pin2, int pin3, PID motor1, PID motor2, PID motor3) {
        this->readPin1 = pin1;
        this->readPin2 = pin2;
        this->readPin3 = pin3;
        this->motor1 = motor1;
        this->motor2 = motor2;
        this->motor3 = motor3;
    }
    void Setup() {
        stdio_init_all();
        gpio_init(this->readPin);
        gpio_set_dir(this->readPin, GPIO_IN);
    }
    int Read1() {
        this->zerod = gpio_get(this->readPin1);
        return this->zerod;
    }
    int Read2() {
        this->zerod = gpio_get(this->readPin2);
        return this->zerod;
    }
    int Read3() {
        this->zerod = gpio_get(this->readPin3);
        return this->zerod;
    }

    void zero() { //put in here for now
        while (!(this->Read1())) {
            this->motor1.update(1);
        }
        while (!(this->Read2())) {
            this->motor2.update(1);
        }
        while (!(this->Read3())) {
            this->motor3.update(1);
        }
    }
  private:
    int readPin1;
    int readPin2;
    int readPin3;
    int zerod = false;
    PID motor1;
    PID motor2;
    PID motor3;
};