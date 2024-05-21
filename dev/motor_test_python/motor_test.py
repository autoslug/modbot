# Import the necessary libraries
import lgpio  # sudo apt install python3-lgpio
from Controller import Controller

# Define the GPIO pins for the motor controller
turn_in1_pin = 22
turn_in2_pin = 27
turn_pwm_pin = 17

wheel_in1_pin = 4
wheel_in2_pin = 3
wheel_pwm_pin = 2

# Define the frequency for the PWM signal
freq = 500

# Initialize the disable flag and the controller
disable = True
joy = Controller()

# Open the GPIO chip
h = lgpio.gpiochip_open(0)

# Claim the GPIO pins as output
lgpio.gpio_claim_output(h, turn_in1_pin)
lgpio.gpio_claim_output(h, turn_in2_pin)
lgpio.gpio_claim_output(h, wheel_in1_pin)
lgpio.gpio_claim_output(h, wheel_in2_pin)

# Initialize the exit counter
exit_count = 0

try:
    # Main loop
    while True:
        # Read the controller status
        status = joy.read_self()

        # If the X button is pressed, disable the motors
        if status.X:
            if not disable:
                print("disabling!")
            disable = True
            exit_count += 1
        # If the B button is pressed, enable the motors
        elif status.B:
            if disable:
                print("enabling!")
            disable = False
            exit_count = 0

        # If the motors are disabled, write 1 to all GPIO pins
        if disable:
            lgpio.gpio_write(h, turn_in1_pin, 1)
            lgpio.gpio_write(h, turn_in2_pin, 1)
            lgpio.gpio_write(h, wheel_in1_pin, 1)
            lgpio.gpio_write(h, wheel_in2_pin, 1)

            # If the exit counter exceeds 50000, break the loop
            if exit_count > 50000:
                break

            continue

        # Read the joystick values
        x = status.LeftJoystickX
        y = status.RightJoystickY

        # Control the turning of the motors based on the X value of the joystick
        if x == 0:
            lgpio.gpio_write(h, turn_in1_pin, 1)
            lgpio.gpio_write(h, turn_in2_pin, 1)
        elif x < 0:
            lgpio.gpio_write(h, turn_in1_pin, 1)
            lgpio.gpio_write(h, turn_in2_pin, 0)
        else:
            lgpio.gpio_write(h, turn_in1_pin, 0)
            lgpio.gpio_write(h, turn_in2_pin, 1)

        # Control the movement of the motors based on the Y value of the joystick
        if y == 0:
            lgpio.gpio_write(h, wheel_in1_pin, 1)
            lgpio.gpio_write(h, wheel_in2_pin, 1)
        elif y < 0:
            lgpio.gpio_write(h, wheel_in1_pin, 1)
            lgpio.gpio_write(h, wheel_in2_pin, 0)
        else:
            lgpio.gpio_write(h, wheel_in1_pin, 0)
            lgpio.gpio_write(h, wheel_in2_pin, 1)

        # Generate the PWM signal for the motors
        lgpio.tx_pwm(h, turn_pwm_pin, freq, round(abs(x) * 100.0, 2))
        lgpio.tx_pwm(h, wheel_pwm_pin, freq, round(abs(y) * 100.0, 2))
except KeyboardInterrupt:
    # If the program is interrupted, pass the exception
    pass

# When the program ends, stop the motors and release the GPIO pins
lgpio.tx_pwm(h, turn_pwm_pin, freq, 0)
lgpio.gpio_write(h, turn_in1_pin, 1)
lgpio.gpio_write(h, turn_in2_pin, 1)

lgpio.tx_pwm(h, wheel_pwm_pin, freq, 0)
lgpio.gpio_write(h, wheel_in1_pin, 1)
lgpio.gpio_write(h, wheel_in2_pin, 1)

lgpio.gpiochip_close(h)
