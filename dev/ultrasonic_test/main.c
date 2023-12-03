/*#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#define trigPin 2
#define echoPin 3

int main()
{
    stdio_init_all();
    gpio_init(trigPin);
    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_init(echoPin);
    gpio_set_dir(echoPin, GPIO_IN);

    while (1)
    {
        long duration, distance;
        gpio_put(trigPin, 0);
        sleep_ms(2);
              // Added this line
        gpio_put(trigPin, 1);
        
        //  delayMicroseconds(1000); - Removed this line
        sleep_ms(10);
         // Added this line
        gpio_put(trigPin, 0);
        
        duration = gpio_get(echoPin);
        distance = (duration / 2) / 29.1;
        // if (distance < 4)
        // {                            // This is where the LED On/Off happens
        //     digitalWrite(led, HIGH); // When the Red condition is met, the Green LED should turn off
        //     digitalWrite(led2, LOW);
        // }
        // else
        // {
        //     digitalWrite(led, LOW);
        //     digitalWrite(led2, HIGH);
        // }
        // if (distance >= 200 || distance <= 0)
        // {
        //     Serial.println("Out of range");
        // }
        // else
        // {
        //     Serial.print(distance);
        //     Serial.println(" cm");
        // }
        printf("%ld, %ld\n", duration, distance);
        sleep_ms(500);
    }
}*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"

/// \tag::hello_uart[]

#define UART_ID uart1
#define BAUD_RATE 9600

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 4
#define UART_RX_PIN 5

int main() {
    stdio_init_all();
    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART

    // Send out a character without any conversions
    char buff[2];
    // printf("here\n");
    while(1){
        // printf("WHILE\n");
        uart_putc_raw(UART_ID, 0x55);
        sleep_ms(100);
        uart_read_blocking(UART_ID, buff, 2);
        printf("%d\n", buff[1] + (buff[0] << 8));
        // char a = uart_getc(UART_ID);
        // char b = uart_getc(UART_ID);
        // char b = 0;
        // printf("size %d\n", sizeof b);
        // printf("%d\n", ((int)(b + (a << 8)))/10);
        sleep_ms(100);
        // printf("%u\n", uart_getc(UART_ID));
    }

    // Send out a character but do CR/LF conversions
    //uart_putc(UART_ID, 'B');

    // Send out a string, with CR/LF conversions
    //uart_puts(UART_ID, " Hello, UART!\n");
}
