#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include <stdio.h>
#include <stdlib.h>

// digital low on in# pins indicates direction, both high is no signal
#define turn_in1_pin 4 // 1A, forward direction
#define turn_in2_pin 5 // 1B, backward direction

// #define motor_pwm_pin 9 // 2A, 2B take up by motor speed
#define turn_pwm_pin 9  // 2A, turn motor speed
#define wheel_pwm_pin 8 // 2B, wheel motor speed
#define pwm_slice 4
#define turn_channel PWM_CHAN_B
#define wheel_channel PWM_CHAN_A

#define wheel_in1_pin 6 // 3A, forward direction
#define wheel_in2_pin 7 // 3B, backard direction

#define freq 500 // note: use clock management frequencies to set frequency
#define duty_cycle 1
#define count_max 65535

void setup()
{ // setup pins for pwm functions
    stdio_init_all();
    gpio_init(turn_in1_pin);
    gpio_init(turn_in2_pin);
    gpio_init(wheel_in1_pin);
    gpio_init(wheel_in2_pin);

    // check if default output signal is 0, for now put this in
    gpio_put(turn_in1_pin, 0);
    gpio_put(turn_in2_pin, 0);
    gpio_put(wheel_in1_pin, 0);
    gpio_put(wheel_in2_pin, 0);

    gpio_set_dir(turn_in1_pin, GPIO_OUT);
    gpio_set_dir(turn_in2_pin, GPIO_OUT);
    gpio_set_dir(wheel_in1_pin, GPIO_OUT);
    gpio_set_dir(wheel_in2_pin, GPIO_OUT);

    gpio_set_function(turn_pwm_pin, GPIO_FUNC_PWM);
    gpio_set_function(wheel_pwm_pin, GPIO_FUNC_PWM);
    pwm_set_wrap(pwm_slice, count_max);
    pwm_set_chan_level(pwm_slice, turn_channel, 0);
    pwm_set_chan_level(pwm_slice, wheel_channel, 0);

    pwm_set_enabled(pwm_slice, true);
}

int main()
{
    setup();
    double x = 0;
    double y = 0.2;

    int xflip = 0;
    int yflip = 0;

    float step = 0.001;

    while (1)
    {
        // turn motor
        if (x == 0)
        { // in1 and in2 are high
            // pwm_set_gpio_level (3, count_max); this method would work, but is iffy, as setting the count value for individual pins update next cycle
            gpio_put(turn_in1_pin, 1);
            gpio_put(turn_in2_pin, 1);
        }
        else if (x < 0)
        { // in1 is high and in2 is low
            gpio_put(turn_in1_pin, 1);
            gpio_put(wheel_in2_pin, 0);
        }
        else
        { // in1 is low and in2 is high
            gpio_put(turn_in2_pin, 1);
            gpio_put(wheel_in1_pin, 0);
        }

        // wheel motor
        if (y == 0)
        { // in1 and in2 are high
            gpio_put(wheel_in1_pin, 1);
            gpio_put(wheel_in2_pin, 1);
        }
        else if (y < 0)
        { // in1 is high and in2 is low
            gpio_put(wheel_in1_pin, 1);
            gpio_put(wheel_in2_pin, 0);
        }
        else
        { // in1 is low and in2 is high
            gpio_put(wheel_in2_pin, 1);
            gpio_put(wheel_in1_pin, 0);
        }

        pwm_set_chan_level(pwm_slice, turn_channel, abs((int)(x * count_max)));
        pwm_set_chan_level(pwm_slice, wheel_channel, abs((int)(y * count_max)));
        // printf("hello world\n");
        if (xflip)
        {
            x -= step;
        }
        else
        {
            x += step;
        }
        if (yflip)
        {
            y -= step;
        }
        else
        {
            y += step;
        }
        if (x == 1 || x == -1)
            xflip = !(xflip);
        if (y == 1 || y == -1)
            yflip = !(yflip);

        printf("x: %f, y: %f\n", x, y);
        sleep_ms(20);
    }
    return 0;
}
