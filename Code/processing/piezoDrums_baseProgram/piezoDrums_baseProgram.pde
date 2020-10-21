import processing.serial.*;

Serial myPort;
int serialVal;

//int[] serialInArray = new int[2];    // Where we'll put what we receive
int serialCount = 0;                 // A count of how many bytes we receive

//SIGNAL VARIABLES
ArrayList<Circle> list_of_circles = new ArrayList<Circle>();
ArrayList<Line> list_of_lines = new ArrayList<Line>();

int maxSignalExtent = 500;
int signalGrowthSpeed = 2;

int globalThreshold = 30;

boolean serial_available = true;

void setup()
{
        size(500, 500);
        printArray(Serial.list());
        String portName = Serial.list()[0]; // find right Serial port from list
        try {
                myPort = new Serial(this, portName, 115200);
        } catch(Exception e) {
                println(e);
                serial_available = false;
        }
}

int a = 100, b= 220, c = 200, g = 0;

void draw()
{
        background(0);
        // ---------------------- DEBUG CODE ------------------------
        /*
           background(a%255, b%100, c%50);
           fill(255-c, 255-a, 255-b, ((a+b+c)/30)%100);
           ellipse(height/2, width/2, g++%width, g%height);
           //println(b);
         */

        objectsFromSerial();
        //----------------------- DRAW CIRCLES ---------------------
        if (list_of_circles.size() > 0) {
                // draw signals
                for (Circle circle : list_of_circles) {
                        if (!circle.stopped)
                                circle.propagateSignal();
                        // circle.checkIntersection();
                        circle.drawSignal();
                        circle.xPos--;
                }

                // destroy signals that have reached limit
                for (int i = 0; i<list_of_circles.size(); i++)
                {
                        if (list_of_circles.get(i).signal_magnitude > maxSignalExtent) {
                                list_of_circles.remove(i);
                                println("remaining objects: " + list_of_circles.size() + list_of_lines.size());
                        }
                }
        }

        //----------------------- DRAW LINES ---------------------
        if (list_of_lines.size() > 0) {
                // draw signals
                for (Line line : list_of_lines) {
                        line.draw();
                        line.xPos--;
                }

                // destroy signals that have reached limit
                for (int i = 0; i<list_of_lines.size(); i++)
                {
                        if (list_of_lines.get(i).xPos < -height) {
                                list_of_lines.remove(i);
                                println("remaining objects: " +  list_of_circles.size() + list_of_lines.size());
                        }
                }
        }

        textAlign(RIGHT, BOTTOM);
        textSize(14);
        fill(255);
        text(list_of_circles.size() + "\n" + mouseX + " " + mouseY, width, height);


        // ------------------------ Read Serial ---------------------------
        serialEvent(myPort);

        // ------------------------ print beat step -------------------
        textAlign(LEFT,BOTTOM);
        int current_16th_step = current_16ths_step();
        if (current_16th_step % 4 == 0)
        {
          textSize(24);
          fill(255,0,0);
        }
        else
        {
          textSize(16);
          fill(255);
        }
        text(str(current_16th_step), 0, height);
}

// ------------------------ KEYS ---------------------------
void keyPressed()
{
        switch (key) {
        case 'a':
                list_of_circles.add(new Circle(width*2/3, height*1/5));
                println("a pressed.");
                a+=10;
                //println("remaining objects: " + (list_of_circles.size() + list_of_lines.size()));
                break;

        case 's':
                list_of_lines.add(new Line(width*2/3));
                println("s pressed.");
                b+=10;
                //println("remaining objects: " + (list_of_circles.size() + list_of_lines.size()));
                for (Circle circle : list_of_circles)
                {
                        circle.stopped = true;
                }
                break;

        case 'd':
                println("d pressed.");
                c+=10;
                break;

        case 'e':
                println("e pressed.");

                break;
        }
}

void objectsFromSerial()
{


        //if (serialInArray[0] > globalThreshold)
        //{
        //  list_of_circles.add(new Circle(width*2/3, height*1/5));
        //  println("remaining objects: " + list_of_circles.size() + list_of_lines.size());
        //}
        //if (serialInArray[1] > globalThreshold)
        //{
        //  list_of_lines.add(new Line(width*2/3));
        //  println("remaining objects: " + list_of_circles.size() + list_of_lines.size());
        //  for (Circle circle : list_of_circles)
        //  {
        //    circle.stopped = true;
        //  }
        //}
}


void serialEvent(Serial myPort)
{
        if (serial_available)
        {

                if (myPort.available() > 0)
                {
                        int serialVal = myPort.read();
                        print(millis() + "\t");
                        println(serialVal);
                }
        }
        //println();
        //int inByte = myPort.read();
        //serialInArray[serialCount] = inByte;
        //serialCount++;

        //if (serialCount > 1)
        //{
        //  serialCount = 0;
        //  print(millis() + " ");
        //  printArray(serialInArray);
        //}
}
