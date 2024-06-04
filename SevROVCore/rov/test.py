import time
import pigpio
import manipulator
import camera
import ms5837
from ms5837 import MS5837


from thruster import Thrusters
# from os import system
# system("sudo pigpiod")
# time.sleep(2)

pi = pigpio.pi()
MS5837(model=ms5837.MODEL_30BA, bus=1)
sensor = ms5837.MS5837()

thrusters = Thrusters(pi, [17, 9, 22, 10, 27, 11], [-1, 1, 1, 1, 1, 1])


def initialize():
    print("Initialize maniplator")
    manipulator.initialize()
#     sensor.init()
    print("Initialize ESC")
    thrusters.initialize()
    #time.sleep(3)   
    camera_angle = 50
    camera.rotate(camera_angle)
    print("End init")


def shutdown():
    print("Shutdown")
    manipulator.release()
    thrusters.off()
    pi.stop()


if __name__ == '__main__':
    if not pi.connected:
        exit(-1)
    initialize()
#    time.sleep(2)
    
    for i in range(100):
        for thruster in range(6):
            thrusters.set_thrust(thruster, i)
            print(i)
            time.sleep(0.001)
    for i in range(100):
        for thruster in range(6):
            thrusters.set_thrust(thruster, 99-i)
            print(99-i)
            time.sleep(0.001)
    for i in range(100):
        for thruster in range(6):
            thrusters.set_thrust(thruster, -i)
            print(-i)
            time.sleep(0.001)
    for i in range(100):
        for thruster in range(6):
            thrusters.set_thrust(thruster, i-99)
            print(i-99)
            time.sleep(0.001) 
    print("Initialization complete")
    # thrusters.set_thrust(5, 100)
    # manipulator.grip(-128)
    #manipulator.rotate(128)
    # camera.rotate(0)
    # for i in range(6):
    #     thrusters.set_thrust(i, -10)
    #thrusters.set_thrust(0, -100)
    input('PRESS ENTER FOR CONTINUE')
    #thrusters.set_thrust(4, 10)
    #manipulator.grip(128)
    #manipulator.rotate(-255)
#     manipulator.rotate(128)
#     time.sleep(1)
#     manipulator.rotate(-128)
#     time.sleep(1)
#     manipulator.rotate(0)
#     manipulator.grip(128)
#     time.sleep(2)
#     manipulator.grip(-128)
#     time.sleep(2)
#     manipulator.grip(0)
#     for i in range(100):
#         sensor.read(ms5837.OSR_256)
#         print(sensor.depth())
#         time.sleep(0.5)
    #manipulator.grip(255)
#     thruster = 5
#     for i in range(100):        
#         thrusters.set_thrust(thruster, i)
#         print(i)
#         time.sleep(0.02)
#     for i in range(100):
#         thrusters.set_thrust(thruster, 99-i)
#         print(99-i)
#         time.sleep(0.02)
#     for i in range(100):
#         thrusters.set_thrust(thruster, -i)
#         print(-i)
#         time.sleep(0.02)
#     for i in range(100):
#         thrusters.set_thrust(thruster, i-99)
#         print(i-99)
#         time.sleep(0.02) 
    thrusters.set_thrust(2, 50)
#     camera.rotate(90)
#     time.sleep(1)
#     camera.rotate(0)
#     time.sleep(1)
#     camera.rotate(-90)
#     time.sleep(1)
#     camera.rotate(50)
#     time.sleep(1)
    
    input('PRESS ENTER FOR EXIT')
    shutdown()
