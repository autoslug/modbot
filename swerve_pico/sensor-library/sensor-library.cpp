#include <pico/stdlib.h>
#include <hardware/gpio.h>

#include "sensor-library.h"

ZeroingSensor::ZeroingSensor(int pin)
{
    this->readPin = pin;
}

void ZeroingSensor::Setup()
{
    stdio_init_all();
    gpio_init(this->readPin);
    gpio_set_dir(this->readPin, GPIO_IN);
}

int ZeroingSensor::Read()
{
    gpio_get(this->readPin);
}