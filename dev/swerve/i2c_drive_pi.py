import fcntl
import os
import time

I2C_PRIM = 0x0703

# open i2c devices (sudo apt install i2c-tools)
i2c_fd = os.open("/dev/i2c-1", os.O_RDWR)

# set the i2c address of pico
pico_address = 0x08
fcntl.ioctl(i2c_fd, I2C_PRIM, pico_address)

# send data to pico
data = [0x01, 0x02, 0x03]  # example data
while True:
    try:
        os.write(i2c_fd, bytes(data))
        time.sleep(0.02)
        print("Sent data to Pico: ", list(data))
    except OSError:
        print("Remote I/O Error")
    # read data from pico
    try:
        incoming_data = os.read(i2c_fd, 1)  # read 3 bytes
        time.sleep(0.02)
        print("Received data from Pico: ", list(incoming_data))
    except TimeoutError:
        print("Timeout Error")
