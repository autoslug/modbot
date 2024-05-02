#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEVICE "/dev/spidev0.0"
#define SPI_SPEED 1000000 // 1MHz

int main()
{
    int spi_fd;
    unsigned char tx_data[] = {0xAA, 0xBB, 0xCC, 0xDD};
    unsigned char rx_data[sizeof(tx_data)];

    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (spi_fd < 0)
    {
        perror("Error opening SPI device");
        return -1;
    }

    int mode = SPI_MODE_0;
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) == -1)
    {
        perror("Error setting SPI mode");
        return -1;
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_SPEED) == -1)
    {
        perror("Error setting SPI speed");
        return -1;
    }

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)tx_data,
        .rx_buf = (unsigned long)rx_data,
        .len = sizeof(tx_data),
        .speed_hz = SPI_SPEED,
        .bits_per_word = 8,
    };

    while (1)
    {
        if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &transfer) == -1)
        {
            perror("Error during SPI message transfer");
            return -1;
        }

        printf("Received data: ");
        for (int i = 0; i < sizeof(rx_data); i++)
        {
            printf(" %02X", rx_data[i]);
        }
        printf("\n");
        sleep(1);
    }
    close(spi_fd);
    return 0;
}