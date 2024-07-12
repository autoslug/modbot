#include <hardware/pwm.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>


#define COUNT_MAX 65535

// class to instantiate swerve module
class PWMControl {
  public:
    // store all pin ids
    // swerveModule is controlled by PID loops, module control is abstracted to PID only!
    PWMControl(int motorPin, int pwmPin, int chan = PWM_CHAN_A, int countMax = COUNT_MAX) {
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
        //code to check & override default values
        if(countMax != COUNT_MAX)
            this->countMax = countMax;
    }

    // initialize all pins
    void Setup() { //PICO SPECIFIC
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
        pwm_set_wrap(this->pwm_slice, count_max);

        // set output signal to 0
        // pwm pins can output anything from 0 to countmax of the pwm_slice (this outputs based on the counter)
        pwm_set_chan_level(this->pwm_slice, this->channel, 0);

        // activate pwm slice
        pwm_set_enabled(this->pwm_slice, true);
    }

    

    //drive function control entirely by PID loops
    void Drive(float drivePercentange) { // currently assuming turn & wheel are from -1 to 1, probably need to change later
        if (drivePercentange == 0) { // in1 and in2 are high
            gpio_put(motor_in1_pin, 1);
            gpio_put(motor_in2_pin, 1);
        } else if (drivePercentange < 0) { // in1 is high and in2 is low
            gpio_put(motor_in1_pin, 1);
            gpio_put(motor_in2_pin, 0);
        } else { // in1 is low and in2 is high
            gpio_put(motor_in2_pin, 1);
            gpio_put(motor_in1_pin, 0);
        }

        // set pwm pin output as % of slice output
        pwm_set_chan_level(pwm_slice, channel, abs((int)(drivePercentange * count_max)));
    }

    private:
    //private variables for storing pins and constants
    int motor_in1_pin;
    int motor_in2_pin;
    int pwm_pin;
    int pwm_slice;
    int channel = PWM_CHAN_A;
    int countMax = COUNT_MAX;
};