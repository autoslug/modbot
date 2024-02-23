#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/i2c_slave.h>
#include <hardware/i2c.h>
#include <hardware/pwm.h>

// Define constants for I2C communication
#define I2C_PICO_ADDR 0x08
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define I2C_PORT i2c0
#define I2C_BAUDRATE 100 * 1000

// Define the length of the data packet
#define I2C_DATA_LENGTH 128

#define MESSAGE_START 0xFA
#define MESSAGE_STOP 0xFB

// digital low on a/b pins indicates direction, both high is no signal (stop)
// pwm signal on pwm pin controls speed

#define wheel_1_pwm 2
#define wheel_1_slice 1
#define wheel_1_channel PWM_CHAN_A
#define wheel_1a 3
#define wheel_1b 4

#define turn_1a 5
#define turn_1b 6
#define turn_1_pwm 7
#define turn_1_slice 3
#define turn_1_channel PWM_CHAN_B

#define wheel_2_pwm 8
#define wheel_2_slice 4
#define wheel_2_channel PWM_CHAN_A
#define wheel_2a 9
#define wheel_2b 10

#define turn_2a 11
#define turn_2b 12
#define turn_2_pwm 13
#define turn_2_slice 6
#define turn_2_channel PWM_CHAN_B

#define turn_3_pwm 16
#define turn_3_slice 0
#define turn_3_channel PWM_CHAN_A
#define turn_3b 17
#define turn_3a 18

#define wheel_3b 19
#define wheel_3a 20
#define wheel_3_pwm 21
#define wheel_3_slice 2
#define wheel_3_channel PWM_CHAN_B

// #define freq 500 // note: use clock management frequencies to set frequency
#define count_max 65535

// Status of the input data
int input_status = 0;

// Last event that occurred
int last_event = 0;

// Index of the current data byte
int data_index = 0;

int count = 0;

// Buffer for the input data
typedef struct dataT
{
    uint8_t data[I2C_DATA_LENGTH];
    uint8_t len;
} *data;

typedef struct buffT
{
    data buff[I2C_DATA_LENGTH];
    uint8_t head;
    uint8_t tail;
    uint8_t full;
} *buff;

buff buffer;
data tmp;

// Initialize the circular buffer
void init_buff(buff *b, data *d)
{
    *b = malloc(sizeof(struct buffT));
    (*b)->head = 0;
    (*b)->tail = 0;
    (*b)->full = 0;
    for (int i = 0; i < I2C_DATA_LENGTH; i++)
    {
        (*b)->buff[i] = malloc(sizeof(struct dataT));
        (*b)->buff[i]->len = 0;
        for (int j = 0; j < I2C_DATA_LENGTH; j++)
        {
            (*b)->buff[i]->data[j] = 0;
        }
    }

    *d = malloc(sizeof(struct dataT));
    (*d)->len = 0;
    for (int i = 0; i < I2C_DATA_LENGTH; i++)
    {
        (*d)->data[i] = 0;
    }
}

// Handler for I2C events
static void i2c_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    switch (event)
    {
    case I2C_SLAVE_RECEIVE:
    {
        // Read the data
        uint8_t tmp_byte = i2c_read_byte_raw(i2c);
        // Check if the data is valid
        if (tmp->len >= I2C_DATA_LENGTH)
        {
            printf("Invalid data %x, len %x\n", tmp_byte, tmp->len);
            break;
        }
        // Store the data
        tmp->data[tmp->len] = tmp_byte;
        tmp->len++;
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
        if (last_event == 1 && !buffer->full)
        {
            if (tmp->data[0] == MESSAGE_START && tmp->data[tmp->len - 1] == MESSAGE_STOP)
            {
                for (int i = 0; i < tmp->len; i++)
                {
                    buffer->buff[buffer->tail]->data[i] = tmp->data[i];
                }
                buffer->buff[buffer->tail]->len = tmp->len;

                // set the input status to ready
                input_status = 1;

                // move the tail of the buffer
                buffer->tail = (buffer->tail + 1) % I2C_DATA_LENGTH;

                // check if the buffer is full
                if (buffer->tail == buffer->head)
                {
                    buffer->full = 1;
                }
                tmp->len = 0;
            }
        }
        else
        {
            // printf("Buffer full, attempted to put this into buffer:\n");
            // for (int i = 0; i < tmp->len; i++)
            // {
            //     printf("%d ", tmp->data[i]);
            // }
            // printf("\n");
            tmp->len = 0;
        }
        break;
    default:
        break;
    }
}

float joyX = 0.0f;
float joyY = 0.0f;

void setup_pins(int a, int b, int pwm, int slice, int channel)
{
    gpio_init(a);
    gpio_init(b);
    gpio_init(pwm);
    gpio_set_dir(a, GPIO_OUT);
    gpio_set_dir(b, GPIO_OUT);
    gpio_set_function(pwm, GPIO_FUNC_PWM);
    pwm_set_wrap(slice, count_max);
    pwm_set_chan_level(slice, channel, 0);
    pwm_set_enabled(slice, true);
}

int main()
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    stdio_init_all();

    // Initialize I2C at 100kHz
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    init_buff(&buffer, &tmp);

    // Set I2C address for Pico
    i2c_slave_init(I2C_PORT, I2C_PICO_ADDR, &i2c_handler);

    // Set up the pins for the motors
    setup_pins(wheel_1a, wheel_1b, wheel_1_pwm, wheel_1_slice, wheel_1_channel);
    setup_pins(wheel_2a, wheel_2b, wheel_2_pwm, wheel_2_slice, wheel_2_channel);
    setup_pins(wheel_3a, wheel_3b, wheel_3_pwm, wheel_3_slice, wheel_3_channel);

    setup_pins(turn_1a, turn_1b, turn_1_pwm, turn_1_slice, turn_1_channel);
    setup_pins(turn_2a, turn_2b, turn_2_pwm, turn_2_slice, turn_2_channel);
    setup_pins(turn_3a, turn_3b, turn_3_pwm, turn_3_slice, turn_3_channel);

    while (true)
    {
        gpio_put(LED_PIN, 1);
        if (input_status == 1)
        {
            input_status = 0;
            uint64_t tmp_float = 0;      // use https://www.h-schmidt.net/FloatConverter/IEEE754.html to convert between float and binary/hex
            for (int i = 7; i >= 0; i--) // bytes are stored in int8 array (64 bits), pico reads backwards
            {
                tmp_float |= buffer->buff[buffer->head]->data[i + 1]; // write byte at a time to tmp_float and shift
                printf("%d ", buffer->buff[buffer->head]->data[i + 1]);
                if (i != 0)
                    tmp_float <<= 8; // preserves order of bits in 64bit int
            }
            printf("\n");
            joyX = *(((float *)&tmp_float)); // convert to interpret bits as float (32 bits)
            joyY = *(((float *)&tmp_float) + 1);
            printf("%f   ", joyX);
            printf("%f\n", joyY); // printing floats in console
            tmp_float = 0;        // clear float
            buffer->head = (buffer->head + 1) % I2C_DATA_LENGTH;
            buffer->full = 0;
        }
        else
        {
            printf("%f   ", joyX);
            printf("%f\n", joyY); // printing floats in console
        }
        gpio_put(LED_PIN, 0);

        if (joyX == 0)
        { // in1 and in2 are high
            gpio_put(turn_1a, 1);
            gpio_put(turn_1b, 1);

            gpio_put(turn_2a, 1);
            gpio_put(turn_2b, 1);

            gpio_put(turn_3a, 1);
            gpio_put(turn_3b, 1);
        }
        else if (joyX < 0)
        { // in1 is high and in2 is low
            gpio_put(turn_1a, 1);
            gpio_put(turn_1b, 0);

            gpio_put(turn_2a, 1);
            gpio_put(turn_2b, 0);

            gpio_put(turn_3a, 1);
            gpio_put(turn_3b, 0);
        }
        else
        { // in1 is low and in2 is high
            gpio_put(turn_1b, 1);
            gpio_put(turn_1a, 0);

            gpio_put(turn_2b, 1);
            gpio_put(turn_2a, 0);

            gpio_put(turn_3b, 1);
            gpio_put(turn_3a, 0);
        }

        // wheel motor
        if (joyY == 0)
        { // in1 and in2 are high
            gpio_put(wheel_1a, 1);
            gpio_put(wheel_1b, 1);

            gpio_put(wheel_2a, 1);
            gpio_put(wheel_2b, 1);

            gpio_put(wheel_3a, 1);
            gpio_put(wheel_3b, 1);
        }
        else if (joyY < 0)
        { // in1 is high and in2 is low
            gpio_put(wheel_1a, 1);
            gpio_put(wheel_1b, 0);

            gpio_put(wheel_2a, 1);
            gpio_put(wheel_2b, 0);

            gpio_put(wheel_3a, 1);
            gpio_put(wheel_3b, 0);
        }
        else
        { // in1 is low and in2 is high
            gpio_put(wheel_1a, 0);
            gpio_put(wheel_1b, 1);

            gpio_put(wheel_2a, 0);
            gpio_put(wheel_2b, 1);

            gpio_put(wheel_3a, 0);
            gpio_put(wheel_3b, 1);
        }

        pwm_set_chan_level(turn_1_slice, turn_1_channel, abs((int)(joyX * count_max)));
        pwm_set_chan_level(wheel_1_slice, wheel_1_channel, abs((int)(joyY * count_max)));

        pwm_set_chan_level(turn_2_slice, turn_2_channel, abs((int)(joyX * count_max)));
        pwm_set_chan_level(wheel_2_slice, wheel_2_channel, abs((int)(joyY * count_max)));

        pwm_set_chan_level(turn_3_slice, turn_3_channel, abs((int)(joyX * count_max)));
        pwm_set_chan_level(wheel_3_slice, wheel_3_channel, abs((int)(joyY * count_max)));

        // sleep_ms(20); // used for debugging - reduces the number of loops w/ no new i2c data
    }

    return 0;
}