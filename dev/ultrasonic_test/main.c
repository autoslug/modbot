#include <stdio.h>
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
        printf("%ld\n", duration);
        sleep_ms(500);
    }
}