#!/usr/bin/env python3

import keyboard as kb

while True:  # making a loop
    try:
        # QUIT BY PRESSING Q ANYTIME
        if kb.is_pressed('q'):
            print('Q pressed, quitting...')
            break

        # get keyboard inputs
        if kb.is_pressed('left'):
            left = 1
            print("Pressed Left")
        else: 
            left = 0

        if kb.is_pressed('right'):
            right = 1
            print("Pressed right")
        else: 
            right = 0

        if kb.is_pressed('up'):
            up = 1
            print("Pressed up")
        else: 
            up = 0

        if kb.is_pressed('down'):
            down = 1
            print("Pressed down")
        else: 
            down = 0

    except:
        pass

  