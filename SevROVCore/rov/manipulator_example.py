import math
import time
import pigpio

pi = pigpio.pi()

pwm_grip  = 100
pwm_rotate = -100

value = math.fabs(pwm_grip)
sign = value / pwm_grip if value != 0 else 0
if sign >= 0:
    pi.write(25, 1)
    pi.write(8, 0)
    pi.write(20, 1)
else:
    pi.write(25, 0)
    pi.write(8, 1)
    pi.write(20, 1)    
pi.set_PWM_dutycycle(12, value)

value = math.fabs(pwm_rotate)
sign = value / pwm_rotate if value != 0 else 0
if sign >= 0:
    pi.write(23, 1)
    pi.write(24, 0)
    pi.write(16, 1)
else:
    pi.write(23, 0)
    pi.write(24, 1)
    pi.write(16, 1)
pi.set_PWM_dutycycle(7, value)

time.sleep(2)
pi.write(25, 0)
pi.write(8, 0)
pi.write(20, 0)
pi.write(23, 0)
pi.write(24, 0)
pi.write(16, 0)
pi.set_PWM_dutycycle(12, 0)
pi.set_PWM_dutycycle(7, 0)