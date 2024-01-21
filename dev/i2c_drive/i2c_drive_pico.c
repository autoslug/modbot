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
#define I2C_DATA_LENGTH 10

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
uint8_t input[I2C_DATA_LENGTH - 2];
float joy[2];

void dump(const void *data, size_t len)
{
    const unsigned char *x = data;
    printf("%02x", x[0]);
    for (size_t k = 1; k < len; k++)
        printf(" %02x", x[k]);
    puts("");
}

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
            printf(""); //can use brackets, need to figure out where to wrap
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
        

        // printf("Input: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", input[0], input[1], input[2], input[3], input[4], input[5], input[6], input[7], input[8], input[9]);
        
        // float jx = *((float *)&tmp_float);
        // printf("tester : <%.3f>\n", jx);
        // printf("len of jx : <%i>\n", sizeof(jx));

        // joy[0] = *((float*)&tmp_float);
        // tmp_float = 0;
        // for (int i = 4; i < 8; i++)
        // {
        //     tmp_float = tmp_float << 8;
        //     tmp_float |= input[i];
        // }
        // dump(&tmp_float, 4);
        // float jy = *((float *)&tmp_float);
        // printf("tester : <%.3f>\n", jy);
        // printf("len of jy : <%i>\n", sizeof(jy));
        // // joy[1] = *((float*)&tmp_float);

        // printf("JoyX: %f, JoyY: %f\n", jx, jy);
        // // printf("JoyX: %.3f, JoyY: %.3f\n", joy[0], joy[1]);

        // uint32_t tester = 0x3f9d70a4;
        // float f_test;
        // f_test = *((float *)&tester);
        // printf("tester : <%.3f>\n", f_test);

        // printf("x JoyX: %x, JoyY: %x\n", joy[0], joy[1]);
        // dump(&joy[0], 4);
        // dump(&joy[1], 4);
        // float x = *((float*)&0x3f7e0000) ;
        // dump(&x, 4);

        
        // set the event status to finished
        // last_event = 0;
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
uint32_t array_to_byte(uint8_t bytearray[])
{
    // char byte = 0b00000000; // start with empty byte
    uint32_t byte = 0;
    for (int i = 0; i < 4; i++)
    {
        byte = (char)(byte | bytearray[i]); // since byte is empty and we only have 0 or 1, we can assign to lsb using | operator which doesn't modify the rest of the byte
        if (i != 31)                        // dont shift on first value, as you are directly writing from the array into byte
            byte = byte << 8;               // shift lsb to the left for new bit in array
    }
    return byte;
}

void byte_to_motor_float(float *output, uint8_t arr[], int num)
{ // output float to assign byte to, array of byte input, number/position of byte in float
    uint32_t byte = array_to_byte(arr);
    // test for endianess
    int x = 1;
    char *y = (char *)&x;
    // cast reference of output to char (shift to 1 byte), set value directly
    if (*y)
        *((unsigned char *)(output) + (3 - num)) = byte; // assignment based on little endianess
    else
        *((unsigned char *)(output) + num) = byte; // assignment based on big endianess
}

int byteoffset = 0;
bool read = false;

float joyX = 0.0f;
float joyY = 0.0f;

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
        if (input_status ==1){
            uint64_t tmp_float = 0;
            for (int i = 7; i >= 0; i--)
            {
                tmp_float |= /* tmp_float | */ input[i];
                if(i!=0)
                    tmp_float<<=8;
                printf("%02x ", input[i]);
            }
            printf("\n");
            // printf("%x\n", tmp_float);
            int x = *(((int*)&tmp_float)+1);
            int y = *(((int*)&tmp_float));
            float f_x = *(((float*)&tmp_float)+1);
            float f_y = *(((float*)&tmp_float)+0);
            printf("%i %x   ",x, x);
            printf("%i %x\n",y, y);
            printf("%f %x   ",f_x, f_x);
            printf("%f %x\n",f_y, f_y);
            // printf("\n");
            tmp_float = 0;
        }
        // printf("Status: %d\n", input_status);
        // if (input_status == 1)
        // {
        //     // print output of array_to_byte(input)
        //     printf("Byte: %d\n", array_to_byte(input));
        //     // Byte order is 0xFF, joyX, joyY, 0xFb, 0x00, looking when 0xFF is read and ending when 0xFb is read
        //     if (array_to_byte(input) == MESSAGE_START)
        //     {
        //         printf("Message start");
        //         read = true;
        //     }
        //     if (read)
        //     {
        //         if (byteoffset < 4)
        //         {
        //             byte_to_motor_float(&joyX, input, byteoffset);
        //         }
        //         else
        //         {
        //             byte_to_motor_float(&joyY, input, byteoffset - 4);
        //         }
        //         byteoffset++;
        //         if (byteoffset >= 8)
        //         {
        //             read = false;
        //             byteoffset = 0;
        //         }
        //     }
        // }
        // printf("JoyX: %f, JoyY: %f\n", joy[0], joy[1]);
    }

    return 0;
}