#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>

class ZeroingSensor{
    ZeroingSensor(int pin){
        this->readPin = pin;
    }

    void Setup() {
        stdio_init_all();
        gpio_init(this->readPin);
        gpio_set_dir(this->readPin, GPIO_IN);
    }

    int Read() {
        gpio_get(this->readPin);
    }
    private:
        int readPin;
};