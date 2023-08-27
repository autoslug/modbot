import numpy as np
import matplotlib.pyplot as plt

# import keyboard
from XboxController import XboxController


def chassisStateToModuleStates(vel, omega, module_distances) -> list:
    states = []

    # print(vel, omega, module_distances)

    for module_distance in module_distances:
        states.append(vel + omega * perpendicular(module_distance))

    # print(states)
    return np.array(states)


def perpendicular(vector):
    return np.array([-vector[1], vector[0]])


def normalizeModules(moduleStates):
    max_speed = max([np.linalg.norm(vec) for vec in moduleStates])
    if max_speed <= 1:
        return moduleStates

    new_states = []
    for state in moduleStates:
        new_states.append(state / max_speed)

    return np.array(new_states)


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
    # module radius, i.e distance of modules from center
    R = 5
    # dt, the delta time of the "animation"
    DT = 0.01
    angle_1 = 3.0 / 6.0 * np.pi
    angle_2 = 7.0 / 6.0 * np.pi
    angle_3 = 11.0 / 6.0 * np.pi
    center_pos = np.array([0, 0])
    modules = np.array(
        [
            [R * np.cos(angle_1) + center_pos[0], R * np.sin(angle_1) + center_pos[1]],
            [R * np.cos(angle_2) + center_pos[0], R * np.sin(angle_2) + center_pos[0]],
            [R * np.cos(angle_3) + center_pos[0], R * np.sin(angle_3) + center_pos[1]],
        ]
    )

    plot_vec([modules[:, 0], modules[:, 1]], origin=[[0, 0, 0], [0, 0, 0]])

    plt.pause(DT)

    omega = 0

    joy = XboxController()

    while True:  # making a loop
        try:
            joy_input = joy.read_self()
            # QUIT BY PRESSING Q ANYTIME
            if joy_input.Back:
                print("Q pressed, quitting...")
                break

            # PS5 controller
            # thresholds = [
            #     100,
            #     155
            # ]

            # fake Xbox controller
            thresholds = [-1000, 1000]

            # get keyboard inputs
            left = 1 if joy_input.LeftJoystickX < thresholds[0] else 0
            right = 1 if joy_input.LeftJoystickX > thresholds[1] else 0
            up = 1 if joy_input.LeftJoystickY < thresholds[0] else 0
            down = 1 if joy_input.LeftJoystickY > thresholds[1] else 0

            # good enough for testing, just very basic 8 directions
            dirs = np.array(
                np.array([-1, 0]) * left
                + np.array([1, 0]) * right
                + np.array([0, 1]) * up
                + np.array([0, -1]) * down
            )

            # if not zero (avoid divide by zero error), make it a unit vector
            if dirs.sum() != 0:
                dirs = np.array(dirs / np.linalg.norm(dirs))

            # W and E to rotate
            if joy_input.RightJoystickX < thresholds[0]:
                if omega < 6:
                    omega += 0.75
            elif joy_input.RightJoystickX > thresholds[1]:
                if omega > -6:
                    omega -= 0.75
            else:
                if omega < 0.75 and omega > -0.75:
                    omega = 0
                elif omega > 0:
                    omega -= 0.75
                elif omega < 0:
                    omega += 0.75

            # spin the modules by the raidans/sec * sec
            angle_1 += omega * DT
            angle_2 += omega * DT
            angle_3 += omega * DT
            modules = np.array(
                [
                    [
                        R * np.cos(angle_1) + center_pos[0],
                        R * np.sin(angle_1) + center_pos[1],
                    ],
                    [
                        R * np.cos(angle_2) + center_pos[0],
                        R * np.sin(angle_2) + center_pos[1],
                    ],
                    [
                        R * np.cos(angle_3) + center_pos[0],
                        R * np.sin(angle_3) + center_pos[1],
                    ],
                ]
            )
            center_pos = np.add(center_pos, dirs)
            # center_pos += dirs
            # print(center_pos, dirs)
            module_dirs = chassisStateToModuleStates(dirs, omega / R, modules)

            module_dirs = normalizeModules(module_dirs)

            # modules += module_dirs
            # print(module_dirs)
            print(modules)

            # i spent too much time writing these three lines of code fml
            plt.xlim(-5 * R, 5 * R)
            plt.ylim(-5 * R, 5 * R)
            plt.gca().set_aspect("equal", adjustable="box")

            # [:,0] is fancy way to get all first elements of the lists
            # plot_vec(
            #     [modules[:, 0], modules[:, 1]],
            #     origin=[
            #         [center_pos[0], center_pos[0], center_pos[0]],
            #         [center_pos[1], center_pos[1], center_pos[1]],
            #     ],
            # )

            center_stretched = np.repeat(np.expand_dims(center_pos, axis=1), 3, axis=1)
            plot_vec(
                np.array([modules[:, 0], modules[:, 1]]) - center_stretched,
                origin=center_stretched,
            )

            plot_vec(
                [2 * module_dirs[:, 0], 2 * module_dirs[:, 1]],
                origin=[modules[:, 0], modules[:, 1]],
                color="r",
            )

            plot_vec(
                module_dirs[0] + module_dirs[1] + module_dirs[2],
                origin=center_pos,
                color="g",
            )

            plt.pause(DT)
            plt.clf()

        except Exception as e:
            print(e)
