import fcntl
import os
import time
from Controller import Controller

I2C_PRIM = 0x0703

# open i2c devices (sudo apt install i2c-tools)
i2c_fd = os.open("/dev/i2c-1", os.O_RDWR)

# set the i2c address of pico
pico_address = 0x08
fcntl.ioctl(i2c_fd, I2C_PRIM, pico_address)

# send data to pico
joy = Controller()
delay = 0.05

while True:
    status = joy.read_self()
    x = status.LeftJoystickX
    y = status.RightJoystickY
    joystickswitch = x>0
    data = [0xFA, int(joystickswitch), int(joystickswitch), 0xFB, 0x00]  # example data
    
    try:
        os.write(i2c_fd, bytes(data))
        time.sleep(delay)
        print("Sent data to Pico: ", list(data))
    except OSError:
        print("Remote I/O Error")
    # read data from pico
    try:
        incoming_data = os.read(i2c_fd, 1)  # read 3 bytes
        time.sleep(delay)
        print("Received data from Pico: ", list(incoming_data))
    except TimeoutError:
        print("Timeout Error")
