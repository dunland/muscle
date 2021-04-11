from numpy.lib.shape_base import _replace_zero_by_x_arrays
import p5
import random
from p5.core.color import color_mode
import pyautogui
import serial
import json
# from . import controls

WIDTH = 400
HEIGHT = 400
WIDTH, HEIGHT = pyautogui.size()  # fullscreen
NUM_OF_LINES = 3
lines_max_height = 10
global_line_acceleration = 1

COLOR_MODE = 'RGB'

USE_SERIAL = True
SERIAL_DEVICE = '/dev/ttyACM0'
SERIAL_PORT = 115200

rectangles = []
lines = []
port = None

step = 0
previous_step = 0
init_step = False
bg_color_shift = False


# ----------------------------- SERIAL FUNCTIONS ----------------------
# init:
def serial_init():
    global USE_SERIAL
    global port

    if USE_SERIAL == True:
        try:
            port = serial.Serial(SERIAL_DEVICE, SERIAL_PORT)
            print("port opened.")
        except Exception as identifier:
            USE_SERIAL = False
            print(identifier)

# read:
# def serial_read():
#     if USE_SERIAL == True:
#         try:
#             # print("port.read() = ", port.read())
#             print("port.readline() = ", port.readline().decode('utf-8'))
#             return port.readline()

#         except Exception as identifier:
#             print(identifier)

# assign effects:


def effects_from_serial():
    global lines
    global global_line_acceleration
    global step

    if USE_SERIAL == True:     
        
        message = port.readline().decode('utf-8')

    # if USE_JSON == True:
        # try deserialize json:
        try:
            json_data = json.loads(message)
            step = int(json_data['score']['score_step'])
            print("recevied json!")
            print(json_data)
            print(json_data['Snare']['wasHit'])

        except Exception as e:
            print("loading json failed.", e, "try parsing string instead")

            # try deserialize string:
            try:
                if "hit" in message:

                    print(message)

                    # perform action according to instrument:
                    if 'Kick' in message:
                        lines.append(Line_Object(HEIGHT/2 + random.uniform(-100, 100),
                                    random.randint(0, 10), random.uniform(-1, 1), random_color_=True))
                        print("Kick hit! new line added at y=",
                            lines[len(lines) - 1].y)

                    elif 'Snare' in message:
                        lines.append(Line_Object(HEIGHT/2 + random.uniform(-100, 100),
                                    random.randint(0, 10), random.uniform(-1, 1), random_color_=True))

                        for line in lines:
                            line.red = (line.red - random.randint(0, 20)) % 255
                            line.red = (line.red - random.randint(0, 10)) % 255
                            line.green = (line.green - random.randint(0, 10)) % 255
                            line.blue = (line.blue - random.randint(0, 10)) % 255

                            line.hue = (line.hue + random.randint(0, 10)) % 255
                        print("Snare hit! changing red value")

                    elif 'Ride' in message:
                        global_line_acceleration += 0.01
                        print("Ride hit!")

                if 'step' in message:
                    step += 1


            except Exception as identifier:
                print("..not successful.", identifier)

        # try:
        #     json_data = json.load(message)
        #     print(json_data)
        # except Exception as e:
        #     print(e)



# --------------------------- SUPPORTIVE FUNCTIONS --------------------
# color functions:
def random_color_rgb():
    return p5.Color(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255), 255)


def random_color_hsb():
    return p5.Color(random.randint(0, 255), 255, 255, 255)

def organise_step_functions():
    global step
    global init_step
    global previous_step
    global bg_color_shift

    if step != previous_step:
        init_step = True

        if init_step == True:
            if step == 1:

                while len(lines) < 30:
                        lines.append(Line_Object(HEIGHT/2 + random.uniform(-100, 100),
                        random.randint(0, lines_max_height), random.uniform(-1, 1), random_color_=True))

                for line in lines:
                    line.h = random.randint(0, 100)
                    line.red = random.randint(0, 255)
                    line.green = random.randint(0, 255)
                    line.blue = random.randint(0, 255)
                    line.hue = random.randint(0, 255)

                print("step =", step)

            elif step == 2:
                bg_color_shift = True

        init_step = False
    previous_step = step

# ---------------------------------- RECTS ----------------------------
class Rect_Object():

    def __init__(self, x_, y_, w_, h_):
        self.x = x_
        self.y = y_
        self.w = w_
        self.h = h_
        print("rect with values ", self.x, self.y, self.w, self.h, " created.")

    def render(self):
        p5.fill(255)
        p5.rect((self.x, self.y), self.w, self.h)


# ---------------------------------- LINES ----------------------------

class Line_Object():

    def __init__(self, y_, h_, speed_, random_color_=False, respawn_=False):
        # position:
        self.y = y_
        self.h = h_
        self.speed = speed_

        # colors:
        self.red = 255
        self.green = 255
        self.blue = 255
        self.hue = 0

        # behavior:
        self.respawn = respawn_

        if random_color_ == True:
            if COLOR_MODE == 'RGB':
                self.c = random_color_rgb()
            elif COLOR_MODE == 'HSB':
                self.c = random_color_hsb()
        else:
            self.c = p5.Color(255, 255, 255)

    def move(self):
        self.y += (self.speed * global_line_acceleration)

        # reset to center, when leaving canvas:
        if self.y - self.h > height or self.y + self.h < 0:
            pass
            # if not self.respawn:
                # self.relocate()

    def relocate(self):
        self.y = height/2 + random.uniform(-100, 100)

    # def line_height_probability():
    #     r_value = random.randint(0, lines_max_height)
    #     (1 - (r_value / lines_max_height))
    #     pass

    def render(self):
        if COLOR_MODE == 'RGB':
            self.c = p5.Color(self.red, self.green, self.blue)
        elif COLOR_MODE == 'HSB':
            self.c = p5.Color(self.hue, 255, 255)
        p5.fill(self.c)
        p5.rect(0, self.y-self.h, width, self.h)

    def __del__(self):
        print("line deleted.")
        if self.respawn == True:
            lines.append(Line_Object(random.randint(0, HEIGHT),
                         random.randint(0, 10), random.uniform(-1, 1)))
            print("new line created.")

# ---------------------------------- SETUP ----------------------------


def setup():

    global rectangles
    global lines

    p5.size(WIDTH, HEIGHT)
    p5.color_mode(COLOR_MODE)

    # rectangles = [Rect_Object(random.randint(0, WIDTH), i*10, i*11, i*12) for i in range(10)]

    lines = [Line_Object(random.randint(0, HEIGHT), random.randint(
        0, lines_max_height), random.uniform(-1, 1)) for i in range(NUM_OF_LINES)]

# --------------------------------- DRAW ------------------------------


def draw():
    global port
    global bg_color_shift

    p5.background(0)
    if bg_color_shift == True:
        if COLOR_MODE == "RGB":
            p5.background(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        if COLOR_MODE == "HSB":
            p5.background(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
    p5.fill(255)

    effects_from_serial()

    for i in range(len(lines)):
        lines[i].move()
        lines[i].render()

    organise_step_functions()

# --------------------------------- CONTROLS --------------------------


def key_pressed():
    global global_line_acceleration
    global lines_max_height
    global bg_color_shift
    global step

    # increase acceleration:
    if key == '+':
        global_line_acceleration += 0.1
        print("global line acc = ", global_line_acceleration)

    # change color:
    elif key == 'c':
        for line in lines:

            line.red = (line.red - random.randint(0, 10)) % 255
            line.green = (line.green - random.randint(0, 10)) % 255
            line.blue = (line.blue - random.randint(0, 10)) % 255

            line.hue = (line.hue + random.randint(0, 10)) % 255

    # change size:
    elif key == 'UP':
        if lines_max_height < 100:
            lines_max_height += 2
            print(lines_max_height)


    # ENTER: next mode → all bigger and random color:
    elif key == 'ENTER':
        step += 1


def mouse_pressed():
    global lines
    lines.append(Line_Object(HEIGHT/2 + random.uniform(-100, 100),
                 random.randint(0, lines_max_height), random.uniform(-1, 1), random_color_=True))
    print("new line added at center +/- 100.")


# --------------------------------- RUN -------------------------------
if __name__ == "__main__":
    serial_init()
    p5.run(frame_rate=60)
