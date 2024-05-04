#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

class Zeroing { //store value of sensor
  public:
    Zeroing(int pin) {
        this->readPin = pin;
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
            // spin
        }
    }
    int readPin;
    int zerod = false;
};