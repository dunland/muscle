# controls.py

import p5

# --------------------------------- CONTROLS --------------------------
def key_pressed():
    global global_line_acceleration
    if key == '+':
        global_line_acceleration += 0.1
        print("global line acc = ", global_line_acceleration)


def mouse_pressed():
    global lines
    lines.append(Line_Object(HEIGHT/2 + random.uniform(-100, 100), random.randint(0,10), random.uniform(-1, 1), random_color_=True))
    print("new line added at center +/- 100.")