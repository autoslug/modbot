import lgpio # sudo apt install python3-lgpio
from Controller import Controller


turn_in1_pin = 17
turn_in2_pin = 27
turn_pwm_pin = 22

wheel_in1_pin = 10
wheel_in2_pin = 9
wheel_pwm_pin = 11

freq = 500
disable = True
joy = Controller()

h = lgpio.gpiochip_open(0)

lgpio.gpio_claim_output(h, turn_in1_pin)
lgpio.gpio_claim_output(h, turn_in2_pin)
lgpio.gpio_claim_output(h, wheel_in1_pin)
lgpio.gpio_claim_output(h, wheel_in2_pin)

exit_count = 0

try:
    while True:
        status = joy.read_self()

        if status.X:
            if not disable:
                print("disabling!")
            disable = True
            exit_count += 1
        elif status.B:
            if disable:
                print("enabling!")
            disable = False
            exit_count = 0

        if disable:
            lgpio.gpio_write(h, turn_in1_pin, 1)
            lgpio.gpio_write(h, turn_in2_pin, 1)
            lgpio.gpio_write(h, wheel_in1_pin, 1)
            lgpio.gpio_write(h, wheel_in2_pin, 1)

            if exit_count > 50000:
                break

            continue

        x = status.LeftJoystickX
        y = status.RightJoystickY

        if x == 0:
            lgpio.gpio_write(h, turn_in1_pin, 1)
            lgpio.gpio_write(h, turn_in2_pin, 1)
        elif x < 0:
            lgpio.gpio_write(h, turn_in1_pin, 1)
            lgpio.gpio_write(h, turn_in2_pin, 0)
        else:
            lgpio.gpio_write(h, turn_in1_pin, 0)
            lgpio.gpio_write(h, turn_in2_pin, 1)

        if y == 0:
            lgpio.gpio_write(h, wheel_in1_pin, 1)
            lgpio.gpio_write(h, wheel_in2_pin, 1)
        elif y < 0:
            lgpio.gpio_write(h, wheel_in1_pin, 1)
            lgpio.gpio_write(h, wheel_in2_pin, 0)
        else:
            lgpio.gpio_write(h, wheel_in1_pin, 0)
            lgpio.gpio_write(h, wheel_in2_pin, 1)

        lgpio.tx_pwm(h, turn_pwm_pin, freq, round(abs(x) * 100.0, 2))
        lgpio.tx_pwm(h, wheel_pwm_pin, freq, round(abs(y) * 100.0, 2))
except KeyboardInterrupt:
    pass

lgpio.tx_pwm(h, turn_pwm_pin, freq, 0)
lgpio.gpio_write(h, turn_in1_pin, 1)
lgpio.gpio_write(h, turn_in2_pin, 1)

lgpio.tx_pwm(h, wheel_pwm_pin, freq, 0)
lgpio.gpio_write(h, wheel_in1_pin, 1)
lgpio.gpio_write(h, wheel_in2_pin, 1)

lgpio.gpiochip_close(h)
