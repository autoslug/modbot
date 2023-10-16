#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"

#include "pio_rotary_encoder.pio.h"

#include "pico/multicore.h"


// class to read the rotation of the rotary encoder
class RotaryEncoder
{
public:
    // constructor
    // rotary_encoder_A is the pin for the A of the rotary encoder.
    // The B of the rotary encoder has to be connected to the next GPIO.
    RotaryEncoder(uint rotary_encoder_A)
    {
        uint8_t rotary_encoder_B = rotary_encoder_A + 1;
        // pio 0 is used
        PIO pio = pio0;
        // state machine 0
        uint8_t sm = 0;
        // configure the used pins as input with pull up
        pio_gpio_init(pio, rotary_encoder_A);
        gpio_set_pulls(rotary_encoder_A, true, false);
        pio_gpio_init(pio, rotary_encoder_B);
        gpio_set_pulls(rotary_encoder_B, true, false);
        // load the pio program into the pio memory
        uint offset = pio_add_program(pio, &pio_rotary_encoder_program);
        // make a sm config
        pio_sm_config c = pio_rotary_encoder_program_get_default_config(offset);
        // set the 'in' pins
        sm_config_set_in_pins(&c, rotary_encoder_A);
        // set shift to left: bits shifted by 'in' enter at the least
        // significant bit (LSB), no autopush
        sm_config_set_in_shift(&c, false, false, 0);
        // set the IRQ handler
        irq_set_exclusive_handler(PIO0_IRQ_0, pio_irq_handler);
        // enable the IRQ
        irq_set_enabled(PIO0_IRQ_0, true);
        pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
        // init the sm.
        // Note: the program starts after the jump table -> initial_pc = 16
        pio_sm_init(pio, sm, 16, &c);
        // enable the sm
        pio_sm_set_enabled(pio, sm, true);
    }

    // set the current rotation to a specific value
    void set_rotation(int _rotation)
    {
        rotation = _rotation;
    }

    // get the current rotation
    int get_rotation(void)
    {
        return rotation;
    }

    // get the current estimated velocity
    int get_velocity(void)
    {
        return velocity;
    }

    void setup_velocity_multicore(void) {
        multicore_launch_core1(velocityLoop);
    }

private:
    void pio_irq_handler()
    {
        // test if irq 0 was raised
        if (pio0_hw->irq & 1)
        {
            rotation = rotation - 1;
        }
        // test if irq 1 was raised
        if (pio0_hw->irq & 2)
        {
            rotation = rotation + 1;
        }
        // clear both interrupts
        pio0_hw->irq = 3;
    }

    void velocityLoop(int delta_time) {
        prev_rotation = rotation
        while (true) {
            sleep_ms(delta_time);
            velocity = (rotation - prev_rotation) / delta_time;
            prev_rotation = rotation
        }
    }

    // the pio instance
    PIO pio;
    // the state machine
    uint sm;
    // the current location of rotation
    int rotation = 0;
    // the last velocity loop rotation recorded
    int prev_rotation = 0;
    // the current estimated velocity
    int velocity = 0;
};

int main()
{
    // needed for printf
    stdio_init_all();
    // the A of the rotary encoder is connected to GPIO 16, B to GPIO 17
    RotaryEncoder my_encoder(16);
    // initialize the rotary encoder rotation as 0
    my_encoder.set_rotation(0);

    my_encoder.setup_velocity_multicore()

    const short LOOP_TIME = 50; // in ms

    int prev_pos = 0;

    // infinite loop to print the current rotation
    while (true)
    {
        sleep_ms(LOOP_TIME);
        
        int pos = my_encoder.get_rotation();

        printf("rotation=%d\n", pos);
        printf("velocity=%d\n", (pos-prev_pos) / LOOP_TIME);
        printf("velocity multicore=%d\n", my_encoder.get_velocity());

        prev_pos = pos;
    }
}