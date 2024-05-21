#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "../pid_control/pid.cpp"

#define COUNT_MAX 65535

// enum for wheel and turn pid loop selection
typedef enum {
    PID_TURN_SELECTION = 0,
    PID_WHEEL_SELECTION,
    PID_BOTH_SELECTION
}PID_Selection;

// class to instantiate swerve module
class SwerveModule {
  public:
    // store all pin ids
    // swerveModule is controlled by PID loops, module control is abstracted to PID only!
    SwerveModule(int turnPin, int wheelPin, int pwmPin, PID wheelPID, PID turnPID, int countMax = COUNT_MAX, int swapPwmChan = 0) {
        // two motors per module, each requiring 3 pins
        // two pins to define h-bridge control and one pin for pwm (since there are 2 motors, there are 2 pwm pins)
        // constructor assumes that all pins are in sets of 2 and will be next to eachother on an even interval (e.g pin 0 and 1, pin 2 and 4, etc)
        this->turn_in1_pin = turnPin; // turn pin set
        this->turn_in2_pin = turnPin + 1;
        this->wheel_in1_pin = wheelPin; // wheel pin set
        this->wheel_in2_pin = wheelPin + 1;
        this->turn_pwm_pin = pwmPin; // pwm pin set
        this->wheel_pwm_pin = pwmPin + 1;
        // pwm slices take up two gpio pins, assuming you take up two pins for pwm with the first pin being even,
        // dividing by 2 is an easier way to get the id without having to specify in constructor
        // the slice is whats activated/outputs a signal and is separate from the pins
        // the pwm pins must be configured to let the signal pass through
        this->pwm_slice = pwmPin / 2;
        //code to check & override default values
        if(countMax != COUNT_MAX)
            this->countMax = countMax;
        if(swapPwmChan){
            this->wheelChan = PWM_CHAN_B;
            this->turnChan = PWM_CHAN_A;
        }
        //PID loop variable setup
        this->wheelPID = wheelPID;
        this->turnPID = turnPID;
    }

    // initialize all pins
    void Setup() { //PICO SPECIFIC
        stdio_init_all();
        // h-bridge requires two binary signals for direction, initialize the in1, in2 pins
        gpio_init(this->turn_in1_pin);
        gpio_init(this->turn_in2_pin);
        gpio_init(this->wheel_in1_pin);
        gpio_init(this->wheel_in2_pin);

        // TODO: check if default output signal is 0, for now put this in
        // sets all output signals to 0
        gpio_put(this->turn_in1_pin, 0);
        gpio_put(this->turn_in2_pin, 0);
        gpio_put(this->wheel_in1_pin, 0);
        gpio_put(this->wheel_in2_pin, 0);

        // define pin output for gpio pins
        gpio_set_dir(this->turn_in1_pin, GPIO_OUT);
        gpio_set_dir(this->turn_in2_pin, GPIO_OUT);
        gpio_set_dir(this->wheel_in1_pin, GPIO_OUT);
        gpio_set_dir(this->wheel_in2_pin, GPIO_OUT);

        // define pin output for pwm pins
        gpio_set_function(this->turn_pwm_pin, GPIO_FUNC_PWM);
        gpio_set_function(this->wheel_pwm_pin, GPIO_FUNC_PWM);

        // set max cycle count of pwm slice (this does counting)
        pwm_set_wrap(this->pwm_slice, count_max);

        // set output signal to 0
        // pwm pins can output anything from 0 to countmax of the pwm_slice (this outputs based on the counter)
        pwm_set_chan_level(this->pwm_slice, this->turn_channel, 0);
        pwm_set_chan_level(this->pwm_slice, this->wheel_channel, 0);

        // activate pwm slice
        pwm_set_enabled(this->pwm_slice, true);
    }

    void updatePID(float turn, float wheel, int selection){
        if(selection == PID_Selection.PID_TURN_SELECTION){
            this->wheelPID.updateTarget(turn);
        }else if(selection == PID_Selection.PID_WHEEL_SELECTION){
            this->turnPID.updateTarget(wheel);
        }else {
            this->wheelPID.updateTarget(turn);
            this->turnPID.updateTarget(wheel);
        }
    }

    

    private:
    //private drive function control entirely by PID loops
    void Drive(float turn, float wheel) { // currently assuming turn & wheel are from -1 to 1, probably need to change later
        if (turn == 0) { // in1 and in2 are high
            gpio_put(turn_in1_pin, 1);
            gpio_put(turn_in2_pin, 1);
        } else if (turn < 0) { // in1 is high and in2 is low
            gpio_put(turn_in1_pin, 1);
            gpio_put(turn_in2_pin, 0);
        } else { // in1 is low and in2 is high
            gpio_put(turn_in2_pin, 1);
            gpio_put(turn_in1_pin, 0);
        }

        // wheel motor
        if (wheel == 0) { // in1 and in2 are high
            gpio_put(wheel_in1_pin, 1);
            gpio_put(wheel_in2_pin, 1);
        } else if (wheel < 0) { // in1 is high and in2 is low
            gpio_put(wheel_in1_pin, 1);
            gpio_put(wheel_in2_pin, 0);
        } else { // in1 is low and in2 is high
            gpio_put(wheel_in1_pin, 0);
            gpio_put(wheel_in2_pin, 1);
        }

        // set pwm pin output as % of slice output
        pwm_set_chan_level(pwm_slice, turn_channel, abs((int)(turn * count_max)));
        pwm_set_chan_level(pwm_slice, wheel_channel, abs((int)(wheel * count_max)));
    }

    //private variables for storing pins and constants
    int turn_in1_pin;
    int turn_in2_pin;
    int wheel_in1_pin;
    int wheel_in2_pin;
    int turn_pwm_pin;
    int wheel_pwm_pin;
    int pwm_slice;
    int wheel_channel = PWM_CHAN_A;
    int turn_channel = PWM_CHAN_B;
    int countMax = COUNT_MAX;
    PID wheelPID;
    PID turnPID;
};