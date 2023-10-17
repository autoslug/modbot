/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
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

class Encoder
{
public:
    Encoder(uint pinA, uint sm, PIO pio, uint offset)
    {
        this->pio = pio;
        this->sm = sm;
        quadrature_encoder_program_init(pio, sm, offset, pinA, 0);
    }

    void update(int delta_time)
    {
        pos = quadrature_encoder_get_count(pio, sm);
        velocity = ((float)(prev_pos - pos)) / delta_time * 360.0 / 374.0 * 1000.0;
        prev_pos = pos;
    }

    int get_pos()
    {
        return pos;
    }

    float get_velocity()
    {
        return velocity;
    }

private:
    int prev_pos, pos;
    float velocity;
    PIO pio;
    uint sm;
};

int
main()
{
    // int new_value_steer, delta_steer, old_value_steer = 0;
    // int new_value_drive, delta_drive, old_value_drive = 0;

    // Base pin to connect the A phase of the encoder.
    // The B phase must be connected to the next pin
    const uint PIN_STEER = 14;
    const uint PIN_DRIVE = 16;

    stdio_init_all();

    // PIO pio = pio0;
    const uint sm_steer = 0;
    const uint sm_drive = 1;

    // we don't really need to keep the offset, as this program must be loaded
    // at offset 0
    uint offset0 = pio_add_program(pio0, &quadrature_encoder_program);
    // quadrature_encoder_program_init(pio0, sm_steer, offset0, PIN_STEER, 0);
    // quadrature_encoder_program_init(pio0, sm_drive, offset0, PIN_DRIVE, 0);
    Encoder steer = Encoder(PIN_STEER, sm_steer, pio0, offset0);
    Encoder drive = Encoder(PIN_DRIVE, sm_drive, pio0, offset0);

    while (1)
    {
        // note: thanks to two's complement arithmetic delta will always
        // be correct even when new_value wraps around MAXINT / MININT
        // new_value_steer = quadrature_encoder_get_count(pio0, sm_steer);
        // new_value_drive = quadrature_encoder_get_count(pio0, sm_drive);

        // delta_steer = new_value_steer - old_value_steer;
        // delta_drive = new_value_drive - old_value_drive;

        // old_value_steer = new_value_steer;
        // old_value_drive = new_value_drive;

        steer.update(20);
        drive.update(20);

        printf("steer position %8d, velocity %6f\n", steer.get_pos(), steer.get_velocity());
        printf("drive position %8d, velocity %6f\n", drive.get_pos(), drive.get_velocity());
        sleep_ms(20);
    }
}
