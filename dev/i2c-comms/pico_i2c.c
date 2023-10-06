#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/i2c_slave.h>
#include <hardware/i2c.h>

#ifndef PICO_DEFAULT_LED_PIN
#warning blink requires a board with a regular LED
#else
const uint LED_PIN = PICO_DEFAULT_LED_PIN;
#endif

uint8_t outgoing_data[3] = {0x11, 0x12, 0x13}; // example data
uint8_t incoming_data[3];
int data_index = 0;

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    switch (event)
    {
    case I2C_SLAVE_RECEIVE: // master has written some data
        for (int i = 0; i < 3; i++)
        {
            if (incoming_data[i] == 0x00)
            {
                incoming_data[i] = i2c_read_byte_raw(i2c);
                printf("Received data %d: 0x%02X\n ", i, incoming_data[i]);
                gpio_put(LED_PIN, 1);
                break;
            }
        }
        break;
    case I2C_SLAVE_REQUEST: // master is requesting data
        i2c_write_byte_raw(i2c, outgoing_data[data_index]);
        printf("Sent data %d: 0x%02X\n ", data_index, outgoing_data[data_index]);
        gpio_put(LED_PIN, 0);
        data_index++;
        if (data_index > 2)
        {
            data_index = 0;
        }
        break;
    case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
        data_index = 0;
        for (int i = 0; i < 3; i++)
        {
            incoming_data[i] = 0x00;
        }
        break;
    default:
        break;
    }
}

int main()
{
    stdio_init_all();

#ifndef PICO_DEFAULT_LED_PIN
#warning blink requires a board with a regular LED
#else
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
#endif

    // init i2c at 100kHz
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);

    // set i2c address for pico
    i2c_slave_init(i2c0, 0x08, &i2c_slave_handler);

    while (1)
        ;

    return 0;
}