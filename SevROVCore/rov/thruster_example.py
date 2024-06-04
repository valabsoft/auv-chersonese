import time
from utils import map_value
import pigpio

#настройка
pi = pigpio.pi()
min_pulse = 1000 #минимальное значение ширины(длительности) сигнала
max_pulse = 2000 #максиамльное значение ширины(длительности) сигнала
pin = 27#пин, к которому подключен мотор

#инициализация
pi.set_servo_pulsewidth(pin, max_pulse)
time.sleep(0.2)
pi.set_servo_pulsewidth(pin, min_pulse)
time.sleep(0.2)
pi.set_servo_pulsewidth(pin, (max_pulse-min_pulse)//2)
time.sleep(2)
print("Initialization complete")
input('PRESS ENTER FOR CONTINUE')

#управление
#thrust = 50 #скорость (-100, 100)
#width = map_value(thrust, -100, 100, min_pulse, max_pulse) #пересчет тре6уемой скорости в сигнал
#pi.set_servo_pulsewidth(pin,width) #передача сигнала на мотор
input('PRESS ENTER FOR CONTINUE')
for i in range(100):
    width = map_value(-i, -100, 100, min_pulse, max_pulse)
    pi.set_servo_pulsewidth(pin,width)
    time.sleep(0.01)
    print(-i)
    print(width)
for i in range(100):
    width = map_value(i-100, -100, 100, min_pulse, max_pulse)
    pi.set_servo_pulsewidth(pin,width)
    time.sleep(0.01)    
    print(i)
    print(width)
for i in range(100):
    width = map_value(i, -100, 100, min_pulse, max_pulse)
    pi.set_servo_pulsewidth(pin,width)
    time.sleep(0.01)
    print(i)
    print(width)
for i in range(100):
    width = map_value(100-i, -100, 100, min_pulse, max_pulse)
    pi.set_servo_pulsewidth(pin,width)
    time.sleep(0.01)
    print(-i)
    print(width)
input('PRESS ENTER FOR CONTINUE')

#отключение мотора
pi.set_servo_pulsewidth(pin,0)
