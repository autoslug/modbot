#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"

#define UART_ID uart1
#define BAUD_RATE 9600

// We are using pins 4 and 5, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 4
#define UART_RX_PIN 5

int ultrasonic_setup()
{
    // Set up our UART with the required speed.

    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    return 0;
}

int ping_time()
{
    char buf[2];  // Send out a character without any conversions
    uart_putc_raw(UART_ID, 0x55); 
    //sends a signal to ultrasonic sensor requesting distance
    //0x50 would request temperature data
    sleep_ms(100); //delay
    uart_read_blocking(UART_ID, buf, 2); //reads data received from sensor
    return buf[1] + (buf[0] << 8); 
    //returns two chars in which one is being bit shifted to the left so when added
    //together would have the bit shifted char in front of the other. 0x34 and 0x21
    //becomes 0x2134.

    // if produces wrong data, a solution could be to swap the 0 and 1.
}

int main()
{
    stdio_init_all();
    if (ultrasonic_setup())
    {
        return 1;
    }

    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART

    while (1)
    {
        printf("%d\n", ping_time());
        sleep_ms(100);
    }
}
