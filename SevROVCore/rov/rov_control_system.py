import numpy as np
import math
from utils import constrain
import asyncio
import re





class RovControlSystem:
    def __init__(self, thrusters, depth_sensor, navx):
        self.thrusters = thrusters
        self.navx = navx
        self.thrust_direction = (0, 0)
        self.depth_sensor = depth_sensor

    def update(self, dt):
        # roll control subsystem
        error_roll = self.navx.roll
        error_pitch = self.navx.pitch
        left_vertical_motor_thrust = error_roll * 20 * dt
        right_vertical_motor_thrust = -left_vertical_motor_thrust
        rear_vertical_motor_thrust = error_pitch * 20 * dt

        self.thrusters.set_thrust_all([left_vertical_motor_thrust, 0,
                                      right_vertical_motor_thrust, 0,
                                     rear_vertical_motor_thrust, 0])

    def set_depth(self, reference_depth):
        current_depth = self.depth_sensor.depth
        depth_error = current_depth - reference_depth
        thrust = constrain(depth_error * 20, -100, 100)
        for i in [1, 3, 6]:
            thruster = self.thrusters[i]
            current_thrust = thruster.thrust
            thruster.set_thrust(current_thrust + thrust)
