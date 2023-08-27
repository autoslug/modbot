import numpy as np
import matplotlib.pyplot as plt
from XboxController import XboxController


def plot_vec(vector, origin=[0, 0], color="black"):
    plt.quiver(
        *origin,
        vector[0],
        vector[1],
        color=color,
        angles="xy",
        scale_units="xy",
        scale=1
    )


if __name__ == "__main__":
    joy = XboxController()

    # robot radius
    R = 5
    # dt, the delta time of the "animation"
    DT = 0.01

    angle_1 = 3.0 / 6.0 * np.pi
    angle_2 = 7.0 / 6.0 * np.pi
    angle_3 = 11.0 / 6.0 * np.pi
    center_pos = np.array([0.0, 0.0])
    module_dirs = np.array([3.0, 7.0, 11.0]) / 6.0 * np.pi
    module_pos = np.array(
        [
            [R * np.cos(a) + center_pos[0], R * np.sin(a) + center_pos[1]]
            for a in module_dirs
        ]
    )

    while True:
        try:
            joy_input = joy.read_self()
            if joy_input.Back:
                print("Exiting")
                break

            # PS5 controller
            # thresholds = [
            #     100,
            #     155
            # ]

            # fake Xbox controller
            thresholds = [-1000, 1000]

            # get inputs
            # left = 1 if joy_input.LeftJoystickX < thresholds[0] else 0
            # right = 1 if joy_input.LeftJoystickX > thresholds[1] else 0
            # up = 1 if joy_input.LeftJoystickY < thresholds[0] else 0
            # down = 1 if joy_input.LeftJoystickY > thresholds[1] else 0
            # cw = 1 if joy_input.RightJoystickX < thresholds[0] else 0
            # ccw = 1 if joy_input.RightJoystickX > thresholds[1] else 0

            # get inputs in "full" resolution to allow for gradual moving & changing directions
            # joystick measures from -32768 to 32767, so we scale it down to -1.0 to 1.0 (change per joystick)
            joy_scale = 2.0**-15

            left_right = round(joy_input.LeftJoystickX * joy_scale, 3)
            up_down = -1.0 * round(joy_input.LeftJoystickY * joy_scale, 3)
            rot = -1.0 * round(joy_input.RightJoystickX * joy_scale, 3)

            # print(left, right, up, down, cw, ccw)
            # print(joy_input.LeftJoystickX, joy_input.LeftJoystickY, joy_input.RightJoystickX)

            ## LOGIC (begin)

            # calculate movement and rotation using inputs
            # old code, only 8 directions
            # move = np.array([right - left, up - down]) * 1.0
            # rotate = cw - ccw
            # new code, full resolution
            move = np.array([left_right, up_down]) * 1.0
            rotate = rot

            # print(move, rotate)

            # update center position
            center_pos += move

            # update module directions
            module_dirs += rotate * 0.1

            ## LOGIC (end)

            # update module positions using module directions and center position
            module_pos = np.array(
                [
                    [R * np.cos(a) + center_pos[0], R * np.sin(a) + center_pos[1]]
                    for a in module_dirs
                ]
            )

            # print(module_pos, module_dirs)

            # set box size and aspect ratio
            box_scale = 10
            plt.xlim(-box_scale * R, box_scale * R)
            plt.ylim(-box_scale * R, box_scale * R)
            plt.gca().set_aspect("equal", adjustable="box")

            # plot robot
            for i, module in enumerate(module_pos):
                # plot line from center to module
                plt.plot(
                    [center_pos[0], module[0]], [center_pos[1], module[1]], "black"
                )

                # calculate module direction vector using robot movement vector & rotation
                dir_vec = (
                    move
                    + np.array([-np.sin(module_dirs[i]), np.cos(module_dirs[i])])
                    * rotate
                )

                # plot module direction vectors
                plt.quiver(
                    module[0],
                    module[1],
                    dir_vec[0],
                    dir_vec[1],
                    color="red",
                    angles="xy",
                    scale_units="xy",
                    scale=0.5,
                )

            # plot center direction vector
            plt.quiver(
                center_pos[0],
                center_pos[1],
                move[0],
                move[1],
                color="green",
                angles="xy",
                scale_units="xy",
                scale=0.5,
            )

            plt.pause(DT)
            plt.clf()

        except Exception as e:
            print(e)
