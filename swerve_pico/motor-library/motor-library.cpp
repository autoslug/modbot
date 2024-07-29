#include <stdio.h>
#include <stdexcept>
#include <pico/stdlib.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"

#include "motor-library.h"
#include "quadrature_encoder.pio.h"

static EncoderFactory factory;

/**
 * file contains syntactic sugar to make getting/setting motors easier
 * created as a lazy way to not have to combine all 3 already working files :/
 */

/**
 * Motor class that holds PWMControl and quadrature_encoder class, encapsulates functions to set and read values of 1 motor
 */
Motor::Motor(int motorPin, int pwmPin, unsigned int encoderPin, float ratio = 1.0, int chan = PWM_CHAN_A, int countMax = COUNT_MAX)
{
    this->pwmControl = PWMControl(motorPin, pwmPin, chan, countMax);
    this->encoder = factory.createEncoder(encoderPin, ratio);
}

void Motor::Setup()
{
    this->pwmControl.Setup();
}

void Motor::SetPwmPercentage(float percentage)
{
    this->pwmControl.Drive(percentage);
}

float Motor::GetPosition()
{
    return this->encoder.get_pos();
}

float Motor::GetVelocity()
{
    return this->encoder.get_velocity();
}

PWMControl::PWMControl(int motorPin, int pwmPin, int chan = PWM_CHAN_A, int countMax = COUNT_MAX)
{
    // two motors per module, each requiring 3 pins
    // two pins to define h-bridge control and one pin for pwm (since there are 2 motors, there are 2 pwm pins)
    // constructor assumes that all pins are in sets of 2 and will be next to eachother on an even interval (e.g pin 0 and 1, pin 2 and 4, etc)
    this->motor_in1_pin = motorPin; // pin set
    this->motor_in2_pin = motorPin + 1;
    this->pwm_pin = pwmPin; // pwm pin set
    // pwm slices take up two gpio pins, assuming you take up two pins for pwm with the first pin being even,
    // the slice is whats activated/outputs a signal and is separate from the pins
    // the pwm pins must be configured to let the signal pass through
    this->pwm_slice = pwm_gpio_to_slice_num(pwmPin);
    this->channel = chan;
    // code to check & override default values
    if (countMax != COUNT_MAX)
        this->countMax = countMax;
}

// initialize all pins
void PWMControl::Setup()
{ // PICO SPECIFIC
    stdio_init_all();
    // h-bridge requires two binary signals for direction, initialize the in1, in2 pins
    gpio_init(this->motor_in1_pin);
    gpio_init(this->motor_in2_pin);

    // TODO: check if default output signal is 0, for now put this in
    // sets all output signals to 0
    gpio_put(this->motor_in1_pin, 0);
    gpio_put(this->motor_in2_pin, 0);

    // define pin output for gpio pins
    gpio_set_dir(this->motor_in1_pin, GPIO_OUT);
    gpio_set_dir(this->motor_in2_pin, GPIO_OUT);

    // define pin output for pwm pins
    gpio_set_function(this->pwm_pin, GPIO_FUNC_PWM);

    // set max cycle count of pwm slice (this does counting)
    pwm_set_wrap(this->pwm_slice, COUNT_MAX);

    // set output signal to 0
    // pwm pins can output anything from 0 to countmax of the pwm_slice (this outputs based on the counter)
    pwm_set_chan_level(this->pwm_slice, this->channel, 0);

    // activate pwm slice
    pwm_set_enabled(this->pwm_slice, true);
}

// drive function control entirely by PID loops
void PWMControl::Drive(float drivePercentange)
{ // currently assuming turn & wheel are from -1 to 1, probably need to change later
    if (drivePercentange == 0)
    { // in1 and in2 are high
        gpio_put(motor_in1_pin, 1);
        gpio_put(motor_in2_pin, 1);
    }
    else if (drivePercentange < 0)
    { // in1 is high and in2 is low
        gpio_put(motor_in1_pin, 1);
        gpio_put(motor_in2_pin, 0);
    }
    else
    { // in1 is low and in2 is high
        gpio_put(motor_in2_pin, 1);
        gpio_put(motor_in1_pin, 0);
    }

    // set pwm pin output as % of slice output
    pwm_set_chan_level(pwm_slice, channel, abs((int)(drivePercentange * COUNT_MAX)));
}


// Create an encoder. Reccommended NOT to use this class, use EncoderFactory::createEncoder()
// @param pinA the pin that encoder A channel is connected to, the B channel should connect to the next pin
// @param sm the state machine to keep track of the encoder, 0-3
// @param which pio
// @param ratio the ratio by which to multiply encoder ticks
Encoder::Encoder(uint pinA, uint sm, PIO pio, float ratio = 1.0, bool addProgram = true)
{
    this->pio = pio;
    this->sm = sm;
    this->ratio = ratio;

    uint offset = 0;

    // we don't really need to keep the offset, as this program must be loaded
    // at offset 0
    if (addProgram)
        uint offset = pio_add_program(pio, &quadrature_encoder_program);

    quadrature_encoder_program_init(pio, sm, offset, pinA, 0);
}

// updates the pos and velocity, call periodically.
// @param delta_time the time, in miliseconds, since last calling update
void Encoder::update(int delta_time)
{
    pos = quadrature_encoder_get_count(pio, sm) * ratio * DEG_PER_ROT;
    velocity = ((prev_pos - pos) / delta_time) * 1000;
    prev_pos = pos;
}

// get position of wheel in ticks, multiplied by any provided ratio. resets on init.
// update() must be called periodically for this to be accurate
float Encoder::get_pos()
{
    return pos;
}

// get velocity of wheel in ticks per second, multiplied by any provided ratio.
// update() must be called periodically for this to be accurate
float Encoder::get_velocity()
{
    return velocity;
}

