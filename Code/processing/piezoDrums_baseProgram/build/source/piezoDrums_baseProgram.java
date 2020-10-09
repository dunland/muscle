import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import processing.serial.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class piezoDrums_baseProgram extends PApplet {



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

public void setup()
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

public void draw()
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
        text(list_of_circles.size() + "\n" + mouseX + " " + mouseY, width, height);


        // ------------------------ Read Serial ---------------------------
        serialEvent(myPort);
}

// ------------------------ KEYS ---------------------------
public void keyPressed()
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

public void objectsFromSerial()
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


public void serialEvent(Serial myPort)
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
int countIntersections = 0;

class Circle
{
  // int pointCounter = 0; // increments for each intersected point; makes sure that the signal does not grow beyond that.

  int signal_magnitude = 0;
  PVector position;
  boolean stopped = false;

  int xPos, yPos;
  Circle(int x_in, int y_in)
  {
    xPos = x_in;
    yPos = y_in;
    position = new PVector(xPos, yPos);
  }

  public void propagateSignal()
  {
    // if (pointCounter < selectedDatapoints.size())
    signal_magnitude += signalGrowthSpeed;
  }

  public void drawSignal()
  {
    noFill();
    stroke(255);
    ellipse(xPos, yPos, signal_magnitude, signal_magnitude);
  }

  public void checkIntersection()
  {
    //for (Datapoint dp : selectedDatapoints) {

    //        if (abs(PVector.dist(this.position, dp.position)) - 3 <= signal_magnitude / 2
    //            && abs(PVector.dist(this.position, dp.position)) + 3 >= signal_magnitude / 2 && !activeDataPoints.contains(dp))
    //        {
    //                // dp.alreadyIntersected = true;
    //                activeDataPoints.add(dp);
    //                countIntersections = activeDataPoints.size();
    //                println("active intersections: " + countIntersections);
    //                dp.intersectionTime = millis();
    //                dp.sound.amp(0.8 / float(countIntersections));
    //                dp.env.play(dp.sound, globalAttackTime, globalSustainTime, globalSustainLevel, globalReleaseTime);
    //                println(millis() + " " + dp.value + " intersect!");
    //                // dp.sound.play(dp.value * data_to_freq_ratio, loudness);
    //                println("loduness = " + 0.8/float(countIntersections));
    //                // pointCounter++;
    //        }


    //}
  }
}

class Line {
  int xPos;
  Line(int x_in)
  {
    xPos = x_in;
  }

  public void draw()
  {
    stroke(255);
    line(xPos, 0, xPos, height);
  }
}
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "piezoDrums_baseProgram" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
