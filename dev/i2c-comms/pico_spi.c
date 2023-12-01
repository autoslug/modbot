#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS 17

int main() {
    stdio_init_all();

    spi_init(SPI_PORT, 1000 * 1000); // init at 1MHz
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI); // set pin to SPI mode

    gpi_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);

    while (1) {
        gpio_put(PIN_CS, 1); // set CS high to indiciate start of communication
        uint8_t rx_data[4];
        spi_read_blocking(SPI_PORT, 0, rx_data, sizeof(rx_data)); // read data from pi
        gpio_put(PIN_CS, 0); // set CS low to indicate end of communication 

        printf("Received: ");
        for (int i = 0; i < sizeof(rx_data); i++) {
            printf(" %02X", rx_data[i]);
        }
        printf("\n");
        sleep_ms(1000);
    }
    return 0;
}