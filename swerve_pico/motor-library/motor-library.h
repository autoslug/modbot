// #pragma once
#include <hardware/pio.h>
#include <hardware/pwm.h>
#include "quadrature_encoder.pio.h"
#include <stdexcept>

#define COUNT_MAX 65535

const float ROT_PER_TICK = 1.0 / (4 * 374.0);
const float PULLEY_RATIO = 0.3185 / 1.528;
const float DEG_PER_ROT = 360.0;



class PWMControl
{
public:
    PWMControl();
    PWMControl(int motorPin, int pwmPin, int chan = PWM_CHAN_A, int countMax = COUNT_MAX);
    void Setup();
    void Drive(float drivePercentange);

private:
    int motor_in1_pin;
    int motor_in2_pin;
    int pwm_pin;
    int pwm_slice;
    int channel = PWM_CHAN_A;
    int countMax = COUNT_MAX;
};


class Encoder
{
public:
    Encoder();
    Encoder(unsigned int pinA, unsigned int sm, PIO pio, float ratio = 1.0, bool addProgram = true);
    void update(int delta_time);
    float get_pos();
    float get_velocity();

private:
    float prev_pos, pos;
    float velocity;
    float ratio;
    PIO pio;
    unsigned int sm;
};

class EncoderFactory
{
public:
    // Create an encoder, automatically configuring the state machine and pio.
    // @param pinA the A encoder channel, the B channel should be connected to the next pin
    // @param ratio the ratio by which to multiply encoder outputs. ratio of 1 results in tick / sec
    static Encoder createEncoder(unsigned int pinA, float ratio = 1.0)
    {
        if (encoder_count > 7)
        {
            throw std::out_of_range("reached encoder limit of 8");
        }

        unsigned int sm = encoder_count % 4;
        PIO pio = encoder_count < 4 ? pio0 : pio1;

        encoder_count++;
        return Encoder(pinA, sm, pio, ratio, sm == 0);
    }

private:
    static unsigned int encoder_count;
};

class Motor
{
public:
    Motor();
    Motor(int motorPin, int pwmPin, unsigned int encoderPin, float ratio = 1.0, int chan = PWM_CHAN_A, int countMax = COUNT_MAX);
    void Setup();
    void SetPwmPercentage(float percentage);
    float GetPosition();
    float GetVelocity();

private:
    PWMControl pwmControl;
    Encoder encoder;
};