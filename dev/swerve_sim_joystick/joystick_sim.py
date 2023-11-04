import numpy as np
import matplotlib.pyplot as plt

# from Controller import GemXboxController
# from Controller import NintendoProController
from Controller import Controller


def perpendicular(vec):
    return np.array([-vec[1], vec[0]])


if __name__ == "__main__":
    # joy = GemXboxController()
    # joy = NintendoProController()
    joy = Controller()

    # robot radius
    R = 5
    # dt, the delta time of the "animation"
    DT = 0.001

    center_pos = np.array([0.0, 0.0])
    module_dirs = np.array([3.0, 7.0, 11.0]) / 6.0 * np.pi
    module_pos = np.array(
        [
            [R * np.cos(a) + center_pos[0], R * np.sin(a) + center_pos[1]]
            for a in module_dirs
        ]
    )
    freeze_pos = center_pos.copy()

    while True:
        try:
            joy_input = joy.read_self()
            if joy_input.Back:
                print("Exiting")
                break

            # get inputs in "full" resolution to allow for gradual moving & changing directions

            inverts = [False, False, False, False]  # Nintendo Pro Controller
            # inverts = [False, True, True] # Gem Xbox Controller

            left_x = (-1.0 if inverts[0] else 1.0) * round(joy_input.LeftJoystickX, 3)
            left_y = (-1.0 if inverts[1] else 1.0) * round(joy_input.LeftJoystickY, 3)
            triggers = joy_input.LeftTrigger - joy_input.RightTrigger

            right_x = (-1.0 if inverts[2] else 1.0) * round(joy_input.RightJoystickX, 3)
            right_y = (-1.0 if inverts[3] else 1.0) * round(joy_input.RightJoystickY, 3)

            ## LOGIC (begin)

            # calculate movement and rotation using inputs

            dist = np.hypot(
                freeze_pos[0] - center_pos[0], freeze_pos[1] - center_pos[1]
            )

            if not joy_input.RightBumper:
                move = np.array([left_x, left_y]) * 1.0
                rotate = 0.1 * triggers
            elif dist > R:
                x = (freeze_pos[0] - center_pos[0]) / dist
                y = (freeze_pos[1] - center_pos[1]) / dist
                move = np.array(
                    [-1.0 * y * left_x + x * left_y, x * left_x + y * left_y]
                )
                rotate = (-1.0 if left_x > 0 else 1.0) * np.hypot(
                    move[0], move[1]
                ) / dist + 0.1 * triggers

            if joy_input.LeftBumper:
                freeze_pos = center_pos.copy()
            else:
                freeze_pos += np.array([right_x, right_y]) * 1.0

            if not joy_input.RightBumper:
                freeze_pos += move * 1.0 + np.array([right_x, right_y]) * 1.0

            # update center position
            center_pos += move

            # update module directions
            module_dirs += rotate

            ## LOGIC (end)

            # update module positions using module directions and center position
            module_pos = np.array(
                [
                    [R * np.cos(a) + center_pos[0], R * np.sin(a) + center_pos[1]]
                    for a in module_dirs
                ]
            )

            # set box size and aspect ratio
            box_scale = 10
            plt.xlim(-box_scale * R, box_scale * R)
            plt.ylim(-box_scale * R, box_scale * R)
            plt.gca().set_aspect("equal", adjustable="box")

            module_controls = []

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
                    * 10
                )

                module_controls.append(
                    (
                        round(np.rad2deg(np.arctan2(dir_vec[1], dir_vec[0])), 3),
                        round(np.hypot(dir_vec[0], dir_vec[1]), 3),
                    )
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

            print(module_controls)

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

            plt.plot(
                [center_pos[0], freeze_pos[0]], [center_pos[1], freeze_pos[1]], "b"
            )

            # rumble if robot is outside of box
            if abs(center_pos[0]) > box_scale * R or abs(center_pos[1]) > box_scale * R:
                joy.controller.send_rumble(False, True, 1)

            plt.pause(DT)
            plt.clf()

        except Exception as e:
            print(e)
