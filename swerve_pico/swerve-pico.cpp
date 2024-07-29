#include <stdio.h>

#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>

#include "messaging-library.h"
#include "control-library.h"
#include "sensor-library.h"

char robotState = 0;


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
//temporary address, make comm protocol to send/recieve messages upon linkage/ on bootup
#define PI_ADDRESS 1

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
uint8_t output[I2C_DATA_LENGTH - 2];

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
        // i2c_write_byte_raw(i2c, (uint8_t)input_status);
        // set the event status to sent
        i2c_write_raw_blocking(i2c, output, I2C_DATA_LENGTH - 2);
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



int main(){
    /**
     * DEFINE AND SETUP MOTORS
     */
    Motor turn_motor1 = Motor(0,0,0);
    Motor drive_motor1 = Motor(0,0,0);
    Motor turn_motor2 = Motor(0,0,0);
    Motor drive_motor2 = Motor(0,0,0);
    Motor turn_motor3 = Motor(0,0,0);
    Motor drive_motor3 = Motor(0,0,0);
    
    PID pid1 = PID(0,0,0, turn_motor1);
    PID pid2 = PID(0,0,0, drive_motor1);
    PID pid3 = PID(0,0,0, turn_motor2);
    PID pid4 = PID(0,0,0, drive_motor2);
    PID pid5 = PID(0,0,0, turn_motor3);
    PID pid6 = PID(0,0,0, drive_motor3);

    SwerveDrive module1 = SwerveDrive(pid1,pid2);
    SwerveDrive module2 = SwerveDrive(pid3,pid4);
    SwerveDrive module3 = SwerveDrive(pid5,pid6);

    ZeroingSensor sensor1 = ZeroingSensor(0);
    ZeroingSensor sensor2 = ZeroingSensor(0);
    ZeroingSensor sensor3 = ZeroingSensor(0);

    //I2C Setup

    stdio_init_all();

    // Initialize I2C at 100kHz
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    // Set I2C address for Pico
    i2c_slave_init(I2C_PORT, I2C_PICO_ADDR, &i2c_handler);


    while(1){
        if(robotState == STATE_READ_RESULT_ARRAY){
            module1.SetPidTargets(resultArray[0],resultArray[1]);
            module2.SetPidTargets(resultArray[2],resultArray[3]);
            module3.SetPidTargets(resultArray[4],resultArray[5]);
        }else if(robotState == STATE_ZERO_MOTORS){
            module1.TogglePids(false);
            module2.TogglePids(false);
            module3.TogglePids(false);
            while(!sensor1.Read()){
                turn_motor1.SetPwmPercentage(0.1);
            }
            turn_motor1.SetPwmPercentage(0);
            while(!sensor2.Read()){
                turn_motor2.SetPwmPercentage(0.1);
            }
            turn_motor2.SetPwmPercentage(0);
            while(!sensor3.Read()){
                turn_motor3.SetPwmPercentage(0.1);
            }
            turn_motor3.SetPwmPercentage(0);

            module1.ResetPids();
            module2.ResetPids();
            module3.ResetPids();

            module1.TogglePids(true);
            module2.TogglePids(true);
            module3.TogglePids(true);
        }else if(robotState == STATE_RETURN_DATA){
            // MessagingWriteMessage("DATAAAAAAAA", output);
            // SEND DATA
        }else if(robotState == STATE_WRITE_HEARTBEAT){
            // MessagingWriteMessage((unsigned char)0x0, (char*)output);
            // SEND DATA
        }
        if(input_status == 1){
            char* resultString;
            MessagingReadBuffer((char*)incoming_data,resultString,I2C_DATA_LENGTH,&robotState);
        }
    }
    return 0;
}