#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/i2c_slave.h>
#include <hardware/i2c.h>
#include <stdlib.h>
#include <hardware/pwm.h>

// Define constants for I2C communication
#define I2C_PICO_ADDR 0x08
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define I2C_PORT i2c0
#define I2C_BAUDRATE 100 * 1000

// Define the length of the data packet
#define I2C_DATA_LENGTH 10

#define MESSAGE_START 0xFA
#define MESSAGE_STOP 0xFB


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

// #define freq 500 // note: use clock management frequencies to set frequency
// #define duty_cycle 1
#define count_max 65535


// Buffer for incoming data
uint8_t incoming_data[I2C_DATA_LENGTH];

// Status of the input data
uint8_t input_status = 0;

// Last event that occurred
int last_event = 0;

// Index of the current data byte
int data_index = 0;

// Buffer for the input data
uint8_t input[I2C_DATA_LENGTH - 2];

// Handler for I2C events
static void i2c_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    switch (event)
    {
    case I2C_SLAVE_RECEIVE:{
        // Read the data
        uint8_t tmp = i2c_read_byte_raw(i2c);
        // Check if the data is valid
        // TODO: probably revert this back to the original, we don't really need the MESSAGE_START stuff
        if ((incoming_data[0] == 0x00 && tmp != MESSAGE_START) || data_index >= I2C_DATA_LENGTH)
        {
            printf("Invalid data %x\n", tmp);
            break;
        }
        // Store the data
        incoming_data[data_index] = tmp;
        // printf("Data: %d\n", incoming_data[data_index]);
        data_index++;
        // set the event status to received
        last_event = 1;
        break;
    }
        

    case I2C_SLAVE_REQUEST: // Pi is requesting data
        // Write the data into the void
        i2c_write_byte_raw(i2c, (uint8_t)input_status);
        // set the event status to sent
        last_event = 2;
        break;

    case I2C_SLAVE_FINISH: // Pi has signalled Stop / Restart
        // if the last event was a receive event and the data is valid
        if (last_event == 1)
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
        break;
    default:
        break;
    }
}


bool read = false;

float joyX = 0.0f;
float joyY = 0.0f;

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
    
    stdio_init_all();

    // Initialize I2C at 100kHz
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    // Set I2C address for Pico
    i2c_slave_init(I2C_PORT, I2C_PICO_ADDR, &i2c_handler);

    setup();//setup for pwm

    while (1)
    {
        if (input_status ==1){
            uint64_t tmp_float = 0; //use https://www.h-schmidt.net/FloatConverter/IEEE754.html to convert between float and binary/hex
            for (int i = 7; i >= 0; i--) //bytes are stored in int8 array (64 bits), pico reads backwards
            {
                tmp_float |= input[i]; //write byte at a time to tmp_float and shift
                if(i!=0)
                    tmp_float<<=8; //preserves order of bits in 64bit int
            }
            joyX = *(((float*)&tmp_float)); //convert to interprit bits as float (32 bits)
            joyY = *(((float*)&tmp_float)+1);
            printf("%f   ", joyX);
            printf("%f\n", joyY); //printing floats in console
            tmp_float = 0; //clear float
        }

        if (joyX == 0)
        { // in1 and in2 are high
            // pwm_set_gpio_level (3, count_max); this method would work, but is iffy, as setting the count value for individual pins update next cycle
            gpio_put(turn_in1_pin, 1);
            gpio_put(turn_in2_pin, 1);
        }
        else if (joyX < 0)
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
        if (joyY == 0)
        { // in1 and in2 are high
            gpio_put(wheel_in1_pin, 1);
            gpio_put(wheel_in2_pin, 1);
        }
        else if (joyY < 0)
        { // in1 is high and in2 is low
            gpio_put(wheel_in1_pin, 1);
            gpio_put(wheel_in2_pin, 0);
        }
        else
        { // in1 is low and in2 is high
            gpio_put(wheel_in1_pin, 0);
            gpio_put(wheel_in2_pin, 1);
        }

        pwm_set_chan_level(pwm_slice, turn_channel, abs((int)(joyX* count_max)));
        pwm_set_chan_level(pwm_slice, wheel_channel, abs((int)(joyY * count_max)));
    }

    return 0;
}