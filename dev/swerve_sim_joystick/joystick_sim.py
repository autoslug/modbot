import matplotlib.pyplot as plt
import numpy as np

# from Controller import GemXboxController
from Controller import NintendoProController
from Controller import Controller


# return the vector perpendicular to the given vector
def perpendicular(vec):
    return np.array([-vec[1], vec[0]])


if __name__ == "__main__":
    # joy = GemXboxController()
    # joy = NintendoProController()
    joy = Controller()

    rumble = type(joy) == NintendoProController

    # robot radius
    R = 5
    # dt, the delta time of the "animation"
    DT = 0.001

    # initial robot state
    center_pos = np.array([0.0, 0.0])  # center position
    module_dirs = (
        np.array([3.0, 7.0, 11.0]) / 6.0 * np.pi
    )  # directions of each module, relative to screen
    module_pos = np.array(
        [
            [R * np.cos(a) + center_pos[0], R * np.sin(a) + center_pos[1]]
            for a in module_dirs
        ]
    )  # absolute positions of each module (as a point)
    freeze_pos = (
        center_pos.copy()
    )  # position to rotate about when right bumper is pressed

    while True:
        try:
            # get inputs
            joy_input = joy.read_self()
            if joy_input.Back:  # exit if back button is pressed
                print("Exiting")
                break

            # TODO: should replace this by standardizing inverts in the Controller.py class
            inverts = [False, False, False, False]  # Nintendo Pro Controller
            # inverts = [False, True, True] # Gem Xbox Controller

            # use joystick inputs to calculate "strafe" movement
            left_x = (-1.0 if inverts[0] else 1.0) * round(joy_input.LeftJoystickX, 3)
            left_y = (-1.0 if inverts[1] else 1.0) * round(joy_input.LeftJoystickY, 3)
            triggers = joy_input.LeftTrigger - joy_input.RightTrigger

            right_x = (-1.0 if inverts[2] else 1.0) * round(joy_input.RightJoystickX, 3)
            right_y = (-1.0 if inverts[3] else 1.0) * round(joy_input.RightJoystickY, 3)

            ## LOGIC (begin)

            # get distance between freeze_pos and center_pos
            dist = np.hypot(
                freeze_pos[0] - center_pos[0], freeze_pos[1] - center_pos[1]
            )

            # if right bumper is not pressed, move robot in direction of joystick & rotate relative to center pos
            if not joy_input.RightBumper:
                move = np.array([left_x, left_y]) * 1.0
                rotate = 0.1 * triggers
            elif (
                dist > R
            ):  # if right bumper is pressed and freeze pos is not "inside" robot, rotate robot around freeze pos
                # calculate vector from freeze to center pos
                x = (freeze_pos[0] - center_pos[0]) / dist
                y = (freeze_pos[1] - center_pos[1]) / dist

                # calculate new center position, moving robot around freeze pos
                # x' = x*cos(theta) - y*sin(theta)
                # y' = x*sin(theta) + y*cos(theta)
                # where theta is the rotation angle, but we can use left_x and left_y as sin(theta) and cos(theta)
                # https://academo.org/demos/rotation-about-point/
                move = np.array(
                    [-1.0 * y * left_x + x * left_y, x * left_x + y * left_y]
                )
                # rotate robot so direction of modules is the same relative to freeze pos, plus some rotation from triggers
                rotate = (-1.0 if left_x > 0 else 1.0) * np.hypot(
                    move[0], move[1]
                ) / dist + 0.1 * triggers

            # if left bumper is pressed, make freeze pos the same as center pos
            if joy_input.LeftBumper:
                freeze_pos = center_pos.copy()
            else:  # if left bumper is not pressed, move freeze pos in direction of right joystick
                freeze_pos += np.array([right_x, right_y]) * 1.0

            # if right bumper is not pressed, move freeze pos in direction of right joystick (relative to center pos)
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

            # set box size and aspect ratio for matplotlib plot window
            box_scale = 10
            plt.xlim(-box_scale * R, box_scale * R)
            plt.ylim(-box_scale * R, box_scale * R)
            plt.gca().set_aspect("equal", adjustable="box")

            # array to store module controls (direction & speed of each module)
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

                # add module direction vector to module_controls as degrees & speed
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

            # plot line from center to freeze pos
            plt.plot(
                [center_pos[0], freeze_pos[0]], [center_pos[1], freeze_pos[1]], "b"
            )

            # rumble if robot is outside of box
            if rumble and (
                abs(center_pos[0]) > box_scale * R or abs(center_pos[1]) > box_scale * R
            ):
                joy.controller.send_rumble(False, True, 1)

            # pause for DT seconds and clear plot
            plt.pause(DT)
            plt.clf()

        except Exception as e:
            print(e)
