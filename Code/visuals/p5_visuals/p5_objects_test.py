from p5 import *

# Example: Two Car objects
myCar1 = None
myCar2 = None

def setup():
    global myCar1, myCar2
    size(200, 200)
    # Parameters go inside the parentheses when the object is constructed.
    myCar1 = Car(Color(255,0,0),0,100,2)
    myCar2 = Car(Color(0,0,255),0,10,1)

def draw():
    global myCar1, myCar2
    background(255)

    myCar1.drive()
    myCar1.display()
    myCar2.drive()
    myCar2.display()

class Car:
    def __init__(self, tempC, tempXpos, tempYpos, tempXspeed):
        self.x = tempC
        self.xpos = tempXpos
        self.ypos = tempYpos
        self.xspeed = tempXspeed

    def display(self):
        stroke(0)
        fill(Color(0, 255, 0))
        rect_mode("CENTER")
        rect((self.xpos, self.ypos),20,10)

    def drive(self):
        self.xpos = self.xpos + self.xspeed
        if self.xpos > width:
            self.xpos = 0

if __name__ == '__main__':
    run()