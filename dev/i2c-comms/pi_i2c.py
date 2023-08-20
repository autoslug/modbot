import fcntl
import os
import time

I2C_PRIM = 0x0703

# open i2c devices
i2c_fd = os.open("/dev/i2c-1", os.O_RDWR)

# set the i2c address of pico
pico_address = 0x08
fcntl.ioctl(i2c_fd, I2C_PRIM, pico_address)

# send data to pico
data = [0x01, 0x02, 0x03]  # example data
while (True):
    os.write(i2c_fd, bytes(data))
    time.sleep(1)
