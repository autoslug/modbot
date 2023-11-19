import fcntl
import os
import time
import struct

from Controller import PS4_Controller

I2C_PRIM = 0x0703  # I2C primary address

# Open i2c devices (sudo apt install i2c-tools)
i2c_fd = os.open("/dev/i2c-1", os.O_RDWR)  # File descriptor for the i2c device

# Set the i2c address of pico
pico_address = 0x08  # Address of the pico on the i2c bus
fcntl.ioctl(
    i2c_fd, I2C_PRIM, pico_address
)  # Set the address of the i2c device to communicate with

# Send data to pico
joy = PS4_Controller()  # Initialize the controller
delay = 0.05  # Delay between each read/write operation

while True:  # Infinite loop
    status = joy.read_self()  # Read the status of the controller
    x = status.LeftJoystickX  # Get the X position of the left joystick
    y = status.RightJoystickY  # Get the Y position of the right joystick
    joystickswitch = x > 0  # Check if the joystick is moved to the right

    x_b = struct.pack('f', x)
    y_b = struct.pack('f', y)

    data = bytes([0xFA]) + \
        x_b + \
        y_b + \
        bytes([0xFB, 0x00])  # Prepare the data to be sent
    # data = [0xFA, int(joystickswitch), int(joystickswitch), 0xFB, 0x00]

    print(len(data))
    print(bytes(data))

    try:
        os.write(i2c_fd, data)  # Write the data to the i2c device
        # os.write(i2c_fd, bytes(data))  # Write the data to the i2c device
        time.sleep(delay)  # Wait for a while
        print("Sent data to Pico: ", list(data))  # Print the data that was sent
    except OSError:
        print(
            "Remote I/O Error"
        )  # Print an error message if there was a problem with the write operation

    # Read data from pico
    try:
        incoming_data = os.read(i2c_fd, 1)  # Read 1 byte from the i2c device
        time.sleep(delay)  # Wait for a while
        print(
            "Received data from Pico: ", list(incoming_data)
        )  # Print the data that was received
    except TimeoutError:
        print("Timeout Error")  # Print an error message if there was a timeout error
