import math
import threading

from inputs import get_gamepad  # Import the get_gamepad function from the inputs module
from procon import ProCon  # Import the ProCon class from the procon module


# This class represents a PS4 Controller
class PS4_Controller(object):
    def __init__(self):
        self.MAX_TRIG_VAL = math.pow(2, 8)  # Maximum value for trigger input
        self.MAX_JOY_VAL = math.pow(2, 7)  # Maximum value for joystick input
        self.THRESHOLD = 0.03  # Threshold for joystick deadzone
        self.reset_vars()  # Reset all controller variables to their initial state
        self.start_thread(())  # Start a new thread to monitor the controller

    # This method resets all controller variables to their initial state
    def reset_vars(self):
        # Initialize all controller variables to 0
        self.LeftJoystickY = 0
        self.LeftJoystickX = 0
        self.RightJoystickY = 0
        self.RightJoystickX = 0
        self.LeftTrigger = 0
        self.RightTrigger = 0
        self.LeftBumper = 0
        self.RightBumper = 0
        self.A = 0
        self.X = 0
        self.Y = 0
        self.B = 0
        self.LeftThumb = 0
        self.RightThumb = 0
        self.Back = 0
        self.Start = 0
        self.LeftDPad = 0
        self.RightDPad = 0
        self.UpDPad = 0
        self.DownDPad = 0

    # This method starts a new thread to monitor the controller
    def start_thread(self, thread_args=()):
        self._monitor_thread = threading.Thread(
            target=self._monitor_controller, args=thread_args
        )
        self._monitor_thread.daemon = (
            True  # Set the thread as a daemon so it will end when the main program ends
        )
        self._monitor_thread.start()  # Start the thread

    # This method returns the current state of all buttons/triggers
    def read(self):
        return [
            self.LeftJoystickY,
            self.LeftJoystickX,
            self.RightJoystickY,
            self.RightJoystickX,
            self.LeftTrigger,
            self.RightTrigger,
            self.LeftBumper,
            self.RightBumper,
            self.A,
            self.B,
            self.X,
            self.Y,
            self.LeftThumb,
            self.RightThumb,
            self.Back,
            self.Start,
            self.LeftDPad,
            self.RightDPad,
            self.UpDPad,
            self.DownDPad,
        ]

    # This method returns the controller object itself
    def read_self(self):
        return self

    # This method applies a threshold to a value
    def threshold(self, val):
        return val - 1.0 if abs(val - 1.0) > self.THRESHOLD else 0

    def _monitor_controller(self):
        while True:
            events = get_gamepad()
            for event in events:
                if event.code == "ABS_Y":
                    self.LeftJoystickY = self.threshold(
                        event.state / self.MAX_JOY_VAL
                    )  # normalize between -1 and 1
                elif event.code == "ABS_X":
                    self.LeftJoystickX = self.threshold(
                        event.state / self.MAX_JOY_VAL
                    )  # normalize between -1 and 1
                elif event.code == "ABS_RY":
                    self.RightJoystickY = self.threshold(
                        event.state / self.MAX_JOY_VAL
                    )  # normalize between -1 and 1
                elif event.code == "ABS_RX":
                    self.RightJoystickX = self.threshold(
                        event.state / self.MAX_JOY_VAL
                    )  # normalize between -1 and 1
                elif event.code == "ABS_Z":
                    self.LeftTrigger = self.threshold(
                        event.state / self.MAX_TRIG_VAL
                    )  # normalize between 0 and 1
                elif event.code == "ABS_RZ":
                    self.RightTrigger = self.threshold(
                        event.state / self.MAX_TRIG_VAL
                    )  # normalize between 0 and 1
                elif event.code == "BTN_TL":
                    self.LeftBumper = event.state
                elif event.code == "BTN_TR":
                    self.RightBumper = event.state
                elif event.code == "BTN_SOUTH":
                    self.A = event.state
                elif event.code == "BTN_NORTH":
                    self.Y = event.state  # previously switched with X
                elif event.code == "BTN_WEST":
                    self.X = event.state  # previously switched with Y
                elif event.code == "BTN_EAST":
                    self.B = event.state
                elif event.code == "BTN_THUMBL":
                    self.LeftThumb = event.state
                elif event.code == "BTN_THUMBR":
                    self.RightThumb = event.state
                elif event.code == "BTN_SELECT":
                    self.Back = event.state
                elif event.code == "BTN_START":
                    self.Start = event.state
                elif event.code == "BTN_TRIGGER_HAPPY1":
                    self.LeftDPad = event.state
                elif event.code == "BTN_TRIGGER_HAPPY2":
                    self.RightDPad = event.state
                elif event.code == "BTN_TRIGGER_HAPPY3":
                    self.UpDPad = event.state
                elif event.code == "BTN_TRIGGER_HAPPY4":
                    self.DownDPad = event.state


# This class represents the Xbox Controller in WRP used for the CPSRC GEM
class Gem_Xbox_Controller(PS4_Controller):
    def __init__(self):
        self.MAX_TRIG_VAL = math.pow(2, 8)  # Maximum value for trigger input
        self.MAX_JOY_VAL = math.pow(2, 15)  # Maximum value for joystick input
        self.THRESHOLD = 0.03  # Threshold for joystick deadzone

        self.reset_vars()  # Reset all controller variables to their initial state
        self.start_thread(())  # Start a new thread to monitor the controller


# This class represents the Nintendo Pro Controller
class Nintendo_Pro_Controller(PS4_Controller):
    def __init__(self):
        self.MAX_TRIG_VAL = math.pow(2, 8)  # Maximum value for trigger input
        self.MAX_JOY_VAL = math.pow(2, 15)  # Maximum value for joystick input
        self.THRESHOLD = 0.1  # Threshold for joystick deadzone
        self.controller = ProCon()  # Initialize the ProCon controller

        self.reset_vars()  # Reset all controller variables to their initial state
        self.start_thread(
            self.procon_callback_func
        )  # Start a new thread to monitor the controller

    # This method is called when the ProCon controller state changes
    def procon_callback_func(self, buttons, l_stick, r_stick, *_):
        # Update the controller variables based on the new state
        # The joystick values are normalized between -1 and 1
        # The threshold method is used to apply a deadband to the joystick values
        # The button values are either 0 or 1
        self.LeftJoystickX = self.threshold(l_stick[0] / self.MAX_JOY_VAL)
        self.LeftJoystickY = self.threshold(l_stick[1] / self.MAX_JOY_VAL)
        self.RightJoystickX = self.threshold(r_stick[0] / self.MAX_JOY_VAL)
        self.RightJoystickY = self.threshold(r_stick[1] / self.MAX_JOY_VAL)
        self.LeftTrigger = self.threshold(buttons[ProCon.Button.ZL])
        self.RightTrigger = self.threshold(buttons[ProCon.Button.ZR])
        self.LeftBumper = buttons[ProCon.Button.L]
        self.RightBumper = buttons[ProCon.Button.R]
        self.A = buttons[ProCon.Button.A]
        self.B = buttons[ProCon.Button.B]
        self.X = buttons[ProCon.Button.X]
        self.Y = buttons[ProCon.Button.Y]
        self.LeftThumb = buttons[ProCon.Button.LS]
        self.RightThumb = buttons[ProCon.Button.RS]
        self.Back = buttons[ProCon.Button.MINUS]
        self.Start = buttons[ProCon.Button.PLUS]
        self.LeftDPad = buttons[ProCon.Button.LEFT]
        self.RightDPad = buttons[ProCon.Button.RIGHT]
        self.UpDPad = buttons[ProCon.Button.UP]
        self.DownDPad = buttons[ProCon.Button.DOWN]


if __name__ == "__main__":
    joy = PS4_Controller()  # Initialize a PS4 controller
    # joy = Gem_Xbox_Controller()  # Initialize a Gem Xbox controller
    # joy = Nintendo_Pro_Controller()  # Initialize a Nintendo Pro controller
    while True:
        try:
            print(joy.read())  # Print the current state of the controller
        except Exception as e:
            print("error!", e)  # Print any errors that occur
            break  # Exit the loop if an error occurs
