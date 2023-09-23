from inputs import get_gamepad
import math
import threading
import hid
import time
from procon import ProCon


class Controller(object):
    def __init__(self):
        self.MAX_TRIG_VAL = math.pow(2, 8)
        self.MAX_JOY_VAL = math.pow(2, 7)
        self.THRESHOLD = 0.03

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

        self._monitor_thread = threading.Thread(
            target=self._monitor_controller, args=()
        )
        self._monitor_thread.daemon = True
        self._monitor_thread.start()

    def read(self):  # return the buttons/triggers that you care about in this methode
        # x = self.LeftJoystickX
        # y = self.LeftJoystickY
        # a = self.A
        # b = self.X # b=1, x=2
        # rb = self.RightBumper
        # return [x, y, a, b, rb]
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

    def read_self(self):
        return self

    def threshold(self, val):
        return val if abs(val) > self.THRESHOLD else 0

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


class GemXboxController(Controller):
    def __init__(self):
        self.MAX_TRIG_VAL = math.pow(2, 8)
        self.MAX_JOY_VAL = math.pow(2, 15)
        self.THRESHOLD = 0.03

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

        self._monitor_thread = threading.Thread(
            target=self._monitor_controller, args=()
        )
        self._monitor_thread.daemon = True
        self._monitor_thread.start()


class NintendoProController(Controller):
    def __init__(self):
        self.MAX_TRIG_VAL = math.pow(2, 8)
        self.MAX_JOY_VAL = math.pow(2, 15)
        self.THRESHOLD = 0.1
        self.controller = ProCon()

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

        self._monitor_thread = threading.Thread(
            target=self.controller.start, args=(self.procon_callback_func,)
        )
        self._monitor_thread.daemon = True
        self._monitor_thread.start()

    def read(self):
        return [
            self.LeftJoystickX,
            self.LeftJoystickY,
            self.RightJoystickX,
            self.RightJoystickY,
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

    def procon_callback_func(self, buttons, l_stick, r_stick, *_):
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
    # joy = Controller()
    joy = NintendoProController()
    while True:
        try:
            print(joy.read())
        except Exception as e:
            print("error!", e)
            break
