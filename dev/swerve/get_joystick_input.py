import pygame.joystick
import time

# Initialize the joystick
# pygame.joystick.init()
# joysticks = [pygame.joystick.Joystick(x)
#              for x in range(pygame.joystick.get_count())]
# joystick = joysticks[0]
# joystick.init()

# axes = joystick.get_numaxes()
# buttons = joystick.get_numbuttons()
# hats = joystick.get_numhats()

while True:
    pygame.joystick.init()
    joysticks = [pygame.joystick.Joystick(x)
                 for x in range(pygame.joystick.get_count())]
    joystick = joysticks[0]
    joystick.init()

    axes = joystick.get_numaxes()
    buttons = joystick.get_numbuttons()
    hats = joystick.get_numhats()
    for i in range(axes):
        axis = joystick.get_axis(i)
        print("Axis {} value: {:>6.3f}".format(i, axis))

    for i in range(buttons):
        button = joystick.get_button(i)
        print("Button {:>2} value: {}".format(i, button))

    for i in range(hats):
        hat = joystick.get_hat(i)
        print("Hat {} value: {}".format(i, str(hat)))
    time.sleep(1)

    # for event in pygame.event.get(): # User did something.
    #     # if event.type == pygame.QUIT: # If user clicked close.
    #     #     done = True # Flag that we are done so we exit this loop.
    #     if event.type == pygame.JOYBUTTONDOWN:
    #         print("Joystick button pressed.")
    #     elif event.type == pygame.JOYBUTTONUP:
    #         print("Joystick button released.")

    # # Get count of joysticks.
    # joystick_count = pygame.joystick.get_count()

    # print("Number of joysticks: {}", joystick_count)

    # # For each joystick:
    # for i in range(joystick_count):
    #     joystick = pygame.joystick.Joystick(i)
    #     joystick.init()

    #     print("Joystick {}", i)

    #     # Get the name from the OS for the controller/joystick.
    #     name = joystick.get_name()
    #     print("Joystick name: {}", name)

    #     # Usually axis run in pairs, up/down for one, and left/right for
    #     # the other.
    #     axes = joystick.get_numaxes()
    #     print("Number of axes: {}", axes)

    #     for i in range(axes):
    #         axis = joystick.get_axis(i)
    #         print("Axis {} value: {:>6.3f}", i, axis)

    #     buttons = joystick.get_numbuttons()
    #     print("Number of buttons: {}", buttons)

    #     for i in range(buttons):
    #         button = joystick.get_button(i)
    #         print("Button {:>2} value: {}", i, button)

    #     hats = joystick.get_numhats()
    #     print("Number of hats: {}", hats)

    #     # Hat position. All or nothing for direction, not a float like
    #     # get_axis(). Position is a tuple of int values (x, y).
    #     for i in range(hats):
    #         hat = joystick.get_hat(i)
    #         print("Hat {} value: {}", i, str(hat))
