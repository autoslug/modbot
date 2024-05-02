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

import math

# radius of swerve drive base in meters
radius = 1


# add two vectors
def add_two_vec(v1, v2):
    # get difference between two vectors, treating one vector as perpendicular to x axis
    theta_diff = v2[1] - v1[1]
    theta_diff = theta_diff

    # since vec1 is x axis, vector 1 contributes in only the x axis
    # when breaking into components, just add vec2 in x to vec1 magnitude to get x
    # vec2 in y to get y
    # sqrt(a^2+b^2) to get magnitude, and arctan(y/x) to get angle relative to x (add to vec1 orientation)
    x_comp = v1[0] + math.cos(theta_diff) * v2[0]
    y_comp = math.sin(theta_diff) * v2[0]
    f_mag = math.sqrt(x_comp**2 + y_comp**2)
    f_angle = math.atan2(y_comp, x_comp) + v1[1]
    f_angle = f_angle
    if f_angle < -math.pi:
        f_angle += 2 * math.pi
    elif f_angle > math.pi:
        f_angle -= 2 * math.pi
    return [f_mag, f_angle]


# input velocity [speed (m/s), orientation (deg)] in local reference frame
# rotation (rad/s)
def convert(v_vec, omega):

    # the vector for each motor in global reference frame is given by adding
    # 1) velocity (relative to global reference frame)
    # 2) rotation vector (vector perpendicular to each motor relative to the body reference frame)
    #    because the frame is circular, magnitude of vector is calculated by V = rw (formula for tangential speed relative to radius and rad/s)

    # setting all vectors to velocity relative to local reference frame
    m1 = [v_vec[0], v_vec[1]]
    m2 = [v_vec[0], v_vec[1] - 2 * math.pi / 3]
    m3 = [v_vec[0], v_vec[1] - 4 * math.pi / 3]

    # create magnitude for each vector
    rot_mag = omega * radius
    dir = 1

    if rot_mag < 0:
        dir *= -1
        rot_mag *= -1
    # add two vectors (in local frame) based on direction and magnitude
    m1 = add_two_vec(m1, [rot_mag, math.pi / 2 * dir])
    m2 = add_two_vec(m2, [rot_mag, math.pi / 2 * dir])
    m3 = add_two_vec(m3, [rot_mag, math.pi / 2 * dir])

    return [m1, m2, m3]


while True:  # Infinite loop
    status = joy.read_self()  # Read the status of the controller
    x = status.LeftJoystickX  # Get the X position of the left joystick
    y = -1 * status.LeftJoystickY  # Get the Y position of the left joystick
    w = status.RightJoystickX  # Get the X position of the right joystick

    v = (math.sqrt(x**2 + y**2), math.atan2(y, x))
    m = convert(v, w)

    m1_v = struct.pack("f", m[0][0])
    m1_w = struct.pack("f", math.degrees(m[0][1]))

    m2_v = struct.pack("f", m[1][0])
    m2_w = struct.pack("f", math.degrees(m[1][1]))

    m3_v = struct.pack("f", m[2][0])
    m3_w = struct.pack("f", math.degrees(m[2][1]))

    data = (
        bytes([0xFA]) + m1_v + m1_w + m2_v + m2_w + m3_v + m3_w + bytes([0xFB])
    )  # Prepare the data to be sent
    # data = (
    #     bytes([0xFA])
    #     + struct.pack("f", 0.5)
    #     + struct.pack("f", 0.5)
    #     + struct.pack("f", 0.5)
    #     + struct.pack("f", 0.5)
    #     + struct.pack("f", 0.5)
    #     + struct.pack("f", 0.5)
    #     + bytes([0xFB])
    # )  # Prepare the data to be sent

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
        # print(
        #     "Received data from Pico: ", list(incoming_data)
        # )  # Print the data that was received
    except TimeoutError:
        print("Timeout Error")  # Print an error message if there was a timeout error
    except OSError:
        print("Remote I/O Error")
