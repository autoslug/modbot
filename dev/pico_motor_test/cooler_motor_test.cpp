// #include <pico/stdlib.h>
// #include <hardware/pwm.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <math.h>

// // digital low on in# pins indicates direction, both high is no signal
// #define steer_in1_pin 4 // 1A, forward direction
// #define steer_in2_pin 5 // 1B, backward direction

// #define drive_in1_pin 6 // 3A, forward direction
// #define drive_in2_pin 7 // 3B, backard direction

// // #define motor_pwm_pin 9 // 2A, 2B take up by motor speed
// #define steer_pwm_pin 9 // 2A, steer motor speed
// #define drive_pwm_pin 8 // 2B, drive motor speed
// #define pwm_slice 4
// #define steer_channel PWM_CHAN_B
// #define drive_channel PWM_CHAN_A

// #define count_max 65535 // number of counts in a cycle --> 1/count_max = freq

// /* class Motor
// {
// public:
//     Motor(uint gpio_dir_pin, uint pwm_pin)
//     {
//         this->pwm_pin = pwm_pin;
//         this->gpio_dir_pin = gpio_dir_pin;

//         // setup pins for pwm functions
//         gpio_init(gpio_dir_pin);
//         gpio_init(gpio_dir_pin + 1);

//         // set non-pwm pins to output
//         gpio_set_dir(gpio_dir_pin, GPIO_OUT);
//         gpio_set_dir(gpio_dir_pin + 1, GPIO_OUT);

//         // setup pwm
//         gpio_set_function(pwm_pin, GPIO_FUNC_PWM);

//         // set pwm slice and channels
//         pwm_set_wrap(pwm_pin / 2, count_max);
//         pwm_set_chan_level(pwm_pin / 2, pwm_pin % 2 == 0 ? PWM_CHAN_A : PWM_CHAN_B, 0);

//         pwm_set_enabled(pwm_pin / 2, true);
//     }

//     void set(int power)
//     {
//         power = power > 1 ? 1 : power;
//         power = power < -1 ? -1 : power;
//         if (power == 0)
//         {
//             // setting both to high stops motor
//             gpio_put(steer_in1_pin, 1);
//             gpio_put(steer_in2_pin, 1);
//         }
//         else
//         {
//             gpio_put(steer_in1_pin, power < 0 ? 1 : 0);
//             gpio_put(steer_in2_pin, power < 0 ? 0 : 1);
//         }

//         pwm_set_chan_level(pwm_pin / 2, pwm_pin % 2 == 0 ? PWM_CHAN_A : PWM_CHAN_B, abs((int)(power * count_max)));
//     }

// private:
//     uint pwm_pin;
//     uint gpio_dir_pin;
// }; */

// int main()
// {
//     // setup stdio for printing
//     stdio_init_all();

//     // setup pins for pwm functions
//     gpio_init(steer_in1_pin);
//     gpio_init(steer_in2_pin);
//     gpio_init(drive_in1_pin);
//     gpio_init(drive_in2_pin);

//     // set non-pwm pins to output
//     gpio_set_dir(steer_in1_pin, GPIO_OUT);
//     gpio_set_dir(steer_in2_pin, GPIO_OUT);
//     gpio_set_dir(drive_in1_pin, GPIO_OUT);
//     gpio_set_dir(drive_in2_pin, GPIO_OUT);

//     // setup pwm
//     gpio_set_function(steer_pwm_pin, GPIO_FUNC_PWM);
//     gpio_set_function(drive_pwm_pin, GPIO_FUNC_PWM);

//     // set pwm slice and channels
//     pwm_set_wrap(pwm_slice, count_max);
//     pwm_set_chan_level(pwm_slice, steer_channel, 0);
//     pwm_set_chan_level(pwm_slice, drive_channel, 0);

//     pwm_set_enabled(pwm_slice, true);

//     // step size for oscillation
//     float step = 0.01;
//     int a = 0;

//     while (1)
//     {
//         float power = sin(a * step);
//         // steer motor
//         if (power == 0)
//         { // in1 and in2 are high
//             gpio_put(steer_in1_pin, 1);
//             gpio_put(steer_in2_pin, 1);
//         }
//         else if (power < 0)
//         { // in1 is high and in2 is low
//             gpio_put(steer_in1_pin, 1);
//             gpio_put(steer_in2_pin, 0);
//         }
//         else
//         { // in1 is low and in2 is high
//             gpio_put(steer_in1_pin, 0);
//             gpio_put(steer_in2_pin, 1);
//         }

//         // drive motor
//         if (power == 0)
//         { // in1 and in2 are high
//             gpio_put(drive_in1_pin, 1);
//             gpio_put(drive_in2_pin, 1);
//         }
//         else if (power < 0)
//         { // in1 is high and in2 is low
//             gpio_put(drive_in1_pin, 1);
//             gpio_put(drive_in2_pin, 0);
//         }
//         else
//         { // in1 is low and in2 is high
//             gpio_put(drive_in1_pin, 0);
//             gpio_put(drive_in2_pin, 1);
//         }

//         // set pwm duty cycle
//         pwm_set_chan_level(pwm_slice, steer_channel, abs((int)(power * count_max)));
//         pwm_set_chan_level(pwm_slice, drive_channel, abs((int)(power * count_max)));

//         printf("power: %f %d\n", power, a);

//         a++;
//         if (a * step >= 6.28)
//             a = 0;

//         sleep_ms(20);
//     }
//     return 0;
// }


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
    double y = 0;

    int xflip = 0;
    int yflip = 0;

    float step = 0.001;

    while (1)
    {
        // // turn motor
        if (x == 0)
        { // in1 and in2 are high
            // pwm_set_gpio_level (3, count_max); this method would work, but is iffy, as setting the count value for individual pins update next cycle
            gpio_put(turn_in1_pin, 1);
            gpio_put(turn_in2_pin, 1);
        }
        else if (x < 0)
        { // in1 is high and in2 is low
            gpio_put(turn_in1_pin, 1);
            gpio_put(turn_in2_pin, 0);
        }
        else
        { // in1 is low and in2 is high
            gpio_put(turn_in2_pin, 1);
            gpio_put(turn_in1_pin, 0);
        }

        // wheel motor
        if (x == 0)
        { // in1 and in2 are high
            gpio_put(wheel_in1_pin, 1);
            gpio_put(wheel_in2_pin, 1);
        }
        else if (x < 0)
        { // in1 is high and in2 is low
            gpio_put(wheel_in1_pin, 1);
            gpio_put(wheel_in2_pin, 0);
        }
        else
        { // in1 is low and in2 is high
            gpio_put(wheel_in2_pin, 1);
            gpio_put(wheel_in1_pin, 0);
        }

        // pwm_set_chan_level(pwm_slice, turn_channel, abs((int)(x * count_max)));
        pwm_set_chan_level(pwm_slice, wheel_channel, abs((int)(x * count_max)));
        // printf("hello world\n");
        if (xflip)
        {
            x -= step;
        }
        else
        {
            x += step;
        }
        // if (xflip)
        // {
        //     x -= step;
        // }
        // else
        // {
        //     x += step;
        // }
        if (x >= 1 || x <= -1)
            xflip = !(xflip);
        // if (x >= 1 || x <= -1)
        //     xflip = !(xflip);

        printf("x: %f, x: %f\n", x, x);
        sleep_ms(20);
    }
    return 0;
}