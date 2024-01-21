#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// digital low on in# pins indicates direction, both high is no signal
#define steer_in1_pin 4 // 1A, forward direction
#define steer_in2_pin 5 // 1B, backward direction

#define drive_in1_pin 6 // 3A, forward direction
#define drive_in2_pin 7 // 3B, backard direction

// #define motor_pwm_pin 9 // 2A, 2B take up by motor speed
#define steer_pwm_pin 9 // 2A, steer motor speed
#define drive_pwm_pin 8 // 2B, drive motor speed
#define pwm_slice 4
#define steer_channel PWM_CHAN_B
#define drive_channel PWM_CHAN_A

#define count_max 65535 // number of counts in a cycle --> 1/count_max = freq

class Motor
{
public:
    Motor(uint pin1, uint pin2, uint pwm_pin, uint slice_num, pwm_chan channel, bool slice = true)
    {
        this->pwm_pin = pwm_pin;
        this->pin1 = pin1;
        this->pin2 = pin2;
        this->slice_num = slice_num;
        this->channel = channel;

        // setup pins for pwm functions
        gpio_init(pin1);
        gpio_init(pin2);

        // set non-pwm pins to output
        gpio_set_dir(pin1, GPIO_OUT);
        gpio_set_dir(pin2, GPIO_OUT);

        // setup pwm
        gpio_set_function(pwm_pin, GPIO_FUNC_PWM);

        // set pwm slice and channel
        if (slice)
            pwm_set_wrap(slice_num, count_max);

        pwm_set_chan_level(slice_num, channel, 0);

        if (slice)
            pwm_set_enabled(slice_num, true);
    }

    void set(float power)
    {
        power = power > 1 ? 1 : power;
        power = power < -1 ? -1 : power;
        printf("input power: %f\n", power);

        if (power == 0)
        { // in1 and in2 are high
            gpio_put(this->pin1, 1);
            gpio_put(this->pin2, 1);
        }
        else if (power < 0)
        { // in1 is high and in2 is low
            gpio_put(this->pin1, 1);
            gpio_put(this->pin2, 0);
        }
        else
        { // in1 is low and in2 is high
            gpio_put(this->pin1, 0);
            gpio_put(this->pin2, 1);
        }
        pwm_set_chan_level(this->slice_num, this->channel, abs((int)(power * count_max)));
    }

private:
    uint pwm_pin;
    uint pin1;
    uint pin2;
    uint slice_num;
    pwm_chan channel;
};

int main()
{
    // setup stdio for printing
    stdio_init_all();

    Motor steer = Motor(steer_in1_pin, steer_in2_pin, steer_pwm_pin, pwm_slice, steer_channel, true);
    Motor drive = Motor(drive_in1_pin, drive_in2_pin, drive_pwm_pin, pwm_slice, drive_channel, false);

    // step size for oscillation
    float step = 0.01;
    int a = 0;

    while (1)
    {
        float power = sin(a * step);

        steer.set(power);
        drive.set(power);

        a++;
        if (a * step >= 6.28)
            a = 0;

        sleep_ms(20);
    }
    return 0;
}
