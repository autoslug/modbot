import fcntl
import os
import time


I2C_PRIM = 0x0703

# edit these two arrays for each i2c device
bus_numbers = [1]
pico_addresses = [0x08]

# open i2c devices (sudo apt install i2c-tools)
i2c_fds = []
for i in range(len(bus_numbers)):
    i2c_fds.append(os.open("/dev/i2c-" + bus_numbers[i], os.O_RDWR))
    fcntl.ioctl(i2c_fds[i], I2C_PRIM, pico_addresses[i])

# send data to pico
data = [0x01, 0x02, 0x03]  # example data
while True:
    for i in range(len(bus_numbers)):
        try:
            os.write(i2c_fds[i], bytes(data))
            time.sleep(0.02)
            print("Sent data to Pico: ", list(data))
        except OSError:
            print("Remote I/O Error")
        # read data from pico
        try:
            incoming_data = os.read(i2c_fds[i], 3)  # read 3 bytes
            time.sleep(0.02)
            print("Received data from Pico: ", list(incoming_data))
        except TimeoutError:
            print("Timeout Error")
