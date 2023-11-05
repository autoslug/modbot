#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/i2c_slave.h>
#include <hardware/i2c.h>

// Define constants for I2C communication
#define I2C_PICO_ADDR 0x08
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define I2C_PORT i2c0
#define I2C_BAUDRATE 100 * 1000

// Define the length of the data packet
#define I2C_DATA_LENGTH 4

#define MESSAGE_START 0xFA
#define MESSAGE_STOP 0xFB

// Buffer for incoming data
uint8_t incoming_data[I2C_DATA_LENGTH];

// Status of the input data
uint8_t input_status = 0;

// Last event that occurred
int last_event = 0;

// Index of the current data byte
int data_index = 0;

// Buffer for the input data
int input[I2C_DATA_LENGTH - 2];

// Handler for I2C events
static void i2c_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    switch (event)
    {
    case I2C_SLAVE_RECEIVE: // Pi has written some data
        // Read the data
        uint8_t tmp = i2c_read_byte_raw(i2c);
        // Check if the data is valid
        // TODO: probably revert this back to the original, we don't really need the MESSAGE_START stuff
        if ((incoming_data[0] == 0x00 && tmp != MESSAGE_START) || data_index >= I2C_DATA_LENGTH)
        {
            break;
        }
        // Store the data
        incoming_data[data_index] = tmp;
        data_index++;
        // set the event status to received
        last_event = 1;
        break;

    case I2C_SLAVE_REQUEST: // Pi is requesting data
        // Write the data into the void
        i2c_write_byte_raw(i2c, (uint8_t)input_status);
        // set the event status to sent
        last_event = 2;
        break;

    case I2C_SLAVE_FINISH: // Pi has signalled Stop / Restart
        // if the last event was a receive event and the data is valid
        if (last_event == 1)
        {
            if (incoming_data[0] == MESSAGE_START && incoming_data[I2C_DATA_LENGTH - 1] == MESSAGE_STOP)
            {
                // move the data into the input array
                for (int i = 0; i < I2C_DATA_LENGTH - 2; i++)
                {
                    input[i] = (int)incoming_data[i + 1];
                }
                // set the input status to ready
                input_status = 1;

                // Reset incoming_data
                for (int i = 0; i < I2C_DATA_LENGTH; i++)
                {
                    incoming_data[i] = 0x00;
                }
            }
            data_index = 0;
        }

        // set the event status to finished
        last_event = 0;
        break;
    default:
        break;
    }
}

// Convert byte to motor double
// int byte_to_motor_double(int input)
// {
//     return (double)input / 255.0 * 2.0 - 1.0;
// }

void byte_to_motor_double(float& output, int arr[], int num){//output float to assign byte to, array of byte input, number/position of byte in float
    char byte = 0b00000000; //start with empty byte
    for(int i=0; i < 8; i++){
        byte = (char)(byte | arr[i]); //since byte is empty and we only have 0 or 1, we can assign to lsb using | operator which doesn't modify the rest of the byte
        if(i!=7) //dont shift on first value, as you are directly writing from the array into byte
        byte = byte<<1; //shift lsb to the left for new bit in array
    }
    *((unsigned char*)(&output)+num) = byte;//cast reference of output to char (shift to 1 byte), set value directly
}

int main()
{
    stdio_init_all();

    // Initialize I2C at 100kHz
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    // Set I2C address for Pico
    i2c_slave_init(I2C_PORT, I2C_PICO_ADDR, &i2c_handler);

    while (1)
    {
        printf("Status: %d\n", input_status);
        if (input_status == 1)
        {
            printf("Input: ");
            for (int i = 0; i < I2C_DATA_LENGTH - 2; i++)
            {
                test = test | input[i];
                test << 1;
                printf("%f ", input[i]);
                byte_to_motor_double(input);
            }
            printf("\n");
        }
        
    }

    return 0;
}