#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

int main() {
    stdio_init_all();

    // init i2c at 100kHz
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    // i2c_pullup_en(i2c0, true);

    // set i2c address for pico
    i2c_set_slave_mode(i2c0, true, 0x08);
    // i2c_set_slave_address(i2c0, 0x08); // address should match pi code

    uint8_t outgoing_data[3] = {0x11, 0x12, 0x13}; // example data
    uint8_t incoming_data[3];

    while(1) {
        // read data from i2c_pi.py
        i2c_read_blocking(i2c0, 0x08, incoming_data, 3, true);
        
        // process data
        for (int i = 0; i < 3; i++) {
            printf("Received data %d: 0x%02X\n ", i, incoming_data[i]);
        }

        // send data to pi
        i2c_write_blocking(i2c0, 0x08, outgoing_data, 3, true);
    }

    return 0;
    
}