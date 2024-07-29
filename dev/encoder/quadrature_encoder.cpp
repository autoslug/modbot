/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdexcept>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"

#include "quadrature_encoder.pio.h"

//
// ---- quadrature encoder interface example
//
// the PIO program reads phase A/B of a quadrature encoder and increments or
// decrements an internal counter to keep the current absolute step count
// updated. At any point, the main code can query the current count by using
// the quadrature_encoder_*_count functions. The counter is kept in a full
// 32 bit register that just wraps around. Two's complement arithmetic means
// that it can be interpreted as a 32-bit signed or unsigned value, and it will
// work anyway.
//
// As an example, a two wheel robot being controlled at 100Hz, can use two
// state machines to read the two encoders and in the main control loop it can
// simply ask for the current encoder counts to get the absolute step count. It
// can also subtract the values from the last sample to check how many steps
// each wheel as done since the last sample period.
//
// One advantage of this approach is that it requires zero CPU time to keep the
// encoder count updated and because of that it supports very high step rates.
//

// 374 pulses per revolution (from the description at https://www.dfrobot.com/product-1462.html)
// 4x because the encoder has 2 sensors each w/ rising & falling edges, so each pulse results in
// 4 counts received (https://deltamotion.com/support/webhelp/rmctools/Controller_Features/Transducer_Basics/Quadrature_Fundamentals.htm)
const float ROT_PER_TICK = 1.0 / (4 * 374.0);
const float PULLEY_RATIO = 0.3185 / 1.528;
const float DEG_PER_ROT = 360.0;

class Encoder
{
public:
    // Create an encoder. Reccommended NOT to use this class, use EncoderFactory::createEncoder()
    // @param pinA the pin that encoder A channel is connected to, the B channel should connect to the next pin
    // @param sm the state machine to keep track of the encoder, 0-3
    // @param which pio
    // @param ratio the ratio by which to multiply encoder ticks
    Encoder(uint pinA, uint sm, PIO pio, float ratio = 1.0, bool addProgram = true)
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
    void update(int delta_time)
    {
        pos = quadrature_encoder_get_count(pio, sm) * ratio * DEG_PER_ROT;
        velocity = ((prev_pos - pos) / delta_time) * 1000;
        prev_pos = pos;
    }

    // get position of wheel in ticks, multiplied by any provided ratio. resets on init.
    // update() must be called periodically for this to be accurate
    float get_pos()
    {
        return pos;
    }

    // get velocity of wheel in ticks per second, multiplied by any provided ratio.
    // update() must be called periodically for this to be accurate
    float get_velocity()
    {
        return velocity;
    }

private:
    float prev_pos, pos;
    float velocity;
    float ratio;
    PIO pio;
    uint sm;
};

class EncoderFactory
{
public:
    // Create an encoder, automatically configuring the state machine and pio.
    // @param pinA the A encoder channel, the B channel should be connected to the next pin
    // @param ratio the ratio by which to multiply encoder outputs. ratio of 1 results in tick / sec
    static Encoder createEncoder(uint pinA, float ratio = 1.0)
    {
        if (encoder_count > 7)
        {
            throw std::out_of_range("reached encoder limit of 8");
        }

        uint sm = encoder_count % 4;
        PIO pio = encoder_count < 4 ? pio0 : pio1;

        encoder_count++;
        return Encoder(pinA, sm, pio, ratio, sm == 0);
    }

private:
    static uint encoder_count;
};

uint EncoderFactory::encoder_count = 0;

int main()
{
    stdio_init_all();

    // Base pin to connect the A phase of the encoder (yellow wire).
    // The B phase must be connected to the next pin (green wire)
    const uint PIN_STEER = 14;
    const uint PIN_DRIVE = 16;

    Encoder steer = EncoderFactory::createEncoder(PIN_STEER, ROT_PER_TICK * DEG_PER_ROT * PULLEY_RATIO);
    Encoder drive = EncoderFactory::createEncoder(PIN_DRIVE, ROT_PER_TICK * DEG_PER_ROT);

    while (1)
    {
        steer.update(20);
        drive.update(20);

        printf("steer position %8f, velocity %6f\n", steer.get_pos(), steer.get_velocity());
        printf("drive position %8f, velocity %6f\n", drive.get_pos(), drive.get_velocity());
        sleep_ms(20);
    }
}
