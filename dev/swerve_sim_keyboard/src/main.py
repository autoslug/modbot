#!/usr/bin/env python3
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

matplotlib.use("Qt5Agg")
import keyboard


#################################
## IMPORTANT: DEPRECATED FILE
## See swerve_sim_joystick
#################################





##############################################
###            USER DEFINED FUNC           ###
##############################################


def chassisStateToModuleStates(vel, omega, module_distances) -> list:
    states = []

    for module_distance in module_distances:
        states.append(vel + omega * perpendicular(module_distance))

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


##################################
###            MAIN            ###
##################################

if __name__ == "__main__":
    # module radius, i.e distance of modules from center
    R = 5
    # dt, the delta time of the "animation"
    DT = 0.01
    angle_1 = 3.0 / 6.0 * np.pi
    angle_2 = 7.0 / 6.0 * np.pi
    angle_3 = 11.0 / 6.0 * np.pi
    modules = (
        np.array(
            [
                [np.cos(angle_1), np.sin(angle_1)],
                [np.cos(angle_2), np.sin(angle_2)],
                [np.cos(angle_3), np.sin(angle_3)],
            ]
        )
        * R
    )

    plot_vec([modules[:, 0], modules[:, 1]], origin=[[0, 0, 0], [0, 0, 0]])

    plt.pause(DT)

    omega = 0

    while True:  # making a loop
        try:
            # QUIT BY PRESSING Q ANYTIME
            if keyboard.is_pressed("q"):
                print("Q pressed, quitting...")
                break

            # get keyboard inputs
            left = 1 if keyboard.is_pressed("left") else 0
            right = 1 if keyboard.is_pressed("right") else 0
            up = 1 if keyboard.is_pressed("up") else 0
            down = 1 if keyboard.is_pressed("down") else 0

            # good enough for testing, just very basic 8 directions
            dir = (
                np.array([-1, 0]) * left
                + np.array([1, 0]) * right
                + np.array([0, 1]) * up
                + np.array([0, -1]) * down
            )

            # if not zero (avoid divide by zero error) then make it a unit vector
            if dir.sum() != 0:
                dir = dir / np.linalg.norm(dir)

            # W and E to rotate
            if keyboard.is_pressed("w"):
                if omega < 6:
                    omega += 0.75
            elif keyboard.is_pressed("e"):
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
            modules = (
                np.array(
                    [
                        [np.cos(angle_1), np.sin(angle_1)],
                        [np.cos(angle_2), np.sin(angle_2)],
                        [np.cos(angle_3), np.sin(angle_3)],
                    ]
                )
                * R
            )
            module_dirs = chassisStateToModuleStates(dir, omega / R, modules)

            module_dirs = normalizeModules(module_dirs) * 2

            # i spent too much time writing these three lines of code fml
            plt.xlim(-2 * R, 2 * R)
            plt.ylim(-2 * R, 2 * R)
            plt.gca().set_aspect("equal", adjustable="box")

            # [:,0] is fancy way to get all first elements of the lists
            plot_vec([modules[:, 0], modules[:, 1]], origin=[[0, 0, 0], [0, 0, 0]])
            plot_vec(
                [2 * module_dirs[:, 0], 2 * module_dirs[:, 1]],
                origin=[modules[:, 0], modules[:, 1]],
                color="r",
            )

            plot_vec(module_dirs[0] + module_dirs[1] + module_dirs[2], color="g")

            plt.pause(DT)
            plt.clf()

        except Exception as error:
            print("")
