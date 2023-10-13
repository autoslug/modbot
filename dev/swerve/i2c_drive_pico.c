#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/i2c_slave.h>
#include <hardware/i2c.h>

// #ifndef PICO_DEFAULT_LED_PIN
// #warning blink requires a board with a regular LED
// #else
// const uint LED_PIN = PICO_DEFAULT_LED_PIN;
// #endif

// define variables
#define I2C_PICO_ADDR 0x08
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define I2C_PORT i2c0
#define I2C_BAUDRATE 100 * 1000

// length of data packet: 1 byte for start, 2 bytes for data, 1 byte for stop
#define I2C_DATA_LENGTH 4

#define MESSAGE_START 0xFA
#define MESSAGE_STOP 0xFB

// data received from Pi, stored as doubles
uint8_t incoming_data[I2C_DATA_LENGTH];

uint8_t input_status = 0; // 0 if input is not ready to send to drive (incomplete), 1 if input is ready to send to drive, 2 if input has been sent to drive and has been erased
int last_event = 0;       // 0 if FINISH, 1 if RECEIVE, 2 if REQUEST
int data_index = 0;

int input[I2C_DATA_LENGTH - 2]; // input data from Pi, stored as ints (doesn't include start and stop bytes)

static void i2c_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    switch (event)
    {
    case I2C_SLAVE_RECEIVE: // pi has written some data
        uint8_t tmp = i2c_read_byte_raw(i2c);
        // printf("Received byte: 0x%02X\n", tmp);
        if ((incoming_data[0] == 0x00 && tmp != MESSAGE_START) || data_index >= I2C_DATA_LENGTH)
        {
            break;
        }
        incoming_data[data_index] = tmp;
        data_index++;
        // printf("Received data %d: 0x%02X\n", data_index, tmp);
        // gpio_put(LED_PIN, 1);
        last_event = 1;
        break;

    case I2C_SLAVE_REQUEST: // pi is requesting data

        i2c_write_byte_raw(i2c, (uint8_t)input_status);
        printf("Sent data %d: 0x%02X\n", 0, input_status);
        // gpio_put(LED_PIN, 0);

        last_event = 2;
        break;

    case I2C_SLAVE_FINISH: // pi has signalled Stop / Restart - called at the end of every receive/request

        // printf("last_event: %d\n", last_event);
        if (last_event == 1)
        {
            if (incoming_data[0] == MESSAGE_START && incoming_data[I2C_DATA_LENGTH - 1] == MESSAGE_STOP)
            {
                // input_status = 0;
                printf("Received complete message\n");

                // convert incoming_data to doubles (doesn't include start and stop bytes)
                for (int i = 0; i < I2C_DATA_LENGTH - 2; i++)
                {
                    input[i] = (int)incoming_data[i + 1];
                }
                input_status = 1;

                // reset incoming_data
                for (int i = 0; i < I2C_DATA_LENGTH; i++)
                {
                    incoming_data[i] = 0x00;
                }
            }
            else
            {
                // input_status = 0;
                printf("Received incomplete message\n");
            }
            data_index = 0;
        }

        last_event = 0;
        break;
    default:
        break;
    }
}

int byte_to_motor_double(int input)
{
    return = (double)input / 255.0 * 2.0 - 1.0;
}

int main()
{
    stdio_init_all();

    // #ifndef PICO_DEFAULT_LED_PIN
    // #warning blink requires a board with a regular LED
    // #else
    //     gpio_init(LED_PIN);
    //     gpio_set_dir(LED_PIN, GPIO_OUT);
    // #endif

    // init i2c at 100kHz
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    // set i2c address for pico
    i2c_slave_init(I2C_PORT, I2C_PICO_ADDR, &i2c_handler);

    while (1)
    {
        printf("Status: %d\n", input_status);
        if (input_status == 1)
        {
            printf("Input: ");
            for (int i = 0; i < I2C_DATA_LENGTH - 2; i++)
            {
                printf("%f ", input[i]);
            }
            printf("\n");
            // input_status = 2;
        }
    }

    return 0;
}