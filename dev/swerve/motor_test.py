import time
import lgpio

in1_pin = 4
in2_pin = 3
pwm_pin = 2

freq = 500

h = lgpio.gpiochip_open(0)
lgpio.gpio_claim_output(h, in1_pin)
lgpio.gpio_claim_output(h, in2_pin)
# lgpio.gpio_claim_output(h, pwm_pin)

try:
    while True:
        lgpio.gpio_write(h, in1_pin, 1)
        lgpio.gpio_write(h, in2_pin, 0)
        
        lgpio.tx_pwm(h, pwm_pin, freq, 0)
        time.sleep(5)

        lgpio.tx_pwm(h, pwm_pin, freq, 50)
        time.sleep(5)
        
        lgpio.tx_pwm(h, pwm_pin, freq, 100)
        time.sleep(5)
except KeyboardInterrupt:
    lgpio.tx_pwm(h, pwm_pin, freq, 0)
    lgpio.gpio_write(h, in1_pin, 1)
    lgpio.gpio_write(h, in2_pin, 1)
    lgpio.gpiochip_close(h)