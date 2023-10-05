#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

int main()
{
    stdio_init_all();

#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
#endif

    // init i2c at 100kHz
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);
    // i2c_pullup_en(i2c0, true);

    // set i2c address for pico
    i2c_set_slave_mode(i2c0, true, 0x08);
    // i2c_set_slave_address(i2c0, 0x08); // address should match pi code

    uint8_t outgoing_data[3] = {0x11, 0x12, 0x13}; // example data
    uint8_t incoming_data[3];

    while (1)
    {
        // reset incoming data
        for (int i = 0; i < 3; i++)
        {
            incoming_data[i] = 0x00;
        }

        // if (i2c_get_read_available(i2c0) < 3)
        // {
        //     printf("No data available\n");
        //     continue;
        // }

        // read data from i2c_pi.py
        // i2c_read_timeout_us(i2c0, 0x0703, incoming_data, 3, false, 50000);
        // i2c_read_raw_blocking(i2c0, incoming_data, 3);
        int read_error = i2c_read_blocking(i2c0, 0x08, incoming_data, 3, true);
        gpio_put(LED_PIN, 1);

        if (incoming_data[0] == 0x00)
        {
            printf("No data received %d\n", read_error);
            continue;
        }

        // sleep_ms(250);

        // process data
        for (int i = 0; i < 3; i++)
        {
            printf("Received data %d: 0x%02X\n ", i, incoming_data[i]);
        }

        // send data to pi
        int write_error = i2c_write_timeout_us(i2c0, 0x08, outgoing_data, 3, false, 50000);
        printf("Write error: %d\n", write_error);
        // i2c_write_raw_blocking(i2c0, outgoing_data, 3);
        // i2c_write_blocking(i2c0, 0x08, outgoing_data, 3, true);
        gpio_put(LED_PIN, 0);
        // sleep_ms(250);
    }

    return 0;
}