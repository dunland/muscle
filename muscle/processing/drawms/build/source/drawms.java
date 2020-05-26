import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class drawms extends PApplet {

//SIGNAL VARIABLES
ArrayList<Circle> list_of_circles = new ArrayList<Circle>();
ArrayList<Line> list_of_lines = new ArrayList<Line>();

int maxSignalExtent = 500;
int signalGrowthSpeed = 2;

public void setup()
{
        size(800,600, P2D);
}

public void draw()
{
        background(0);
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

        textAlign(RIGHT,BOTTOM);
        text(list_of_circles.size() + "\n" + mouseX + " " + mouseY, width, height);
}

public void keyPressed()
{
        switch (key) {
        case 'a':
                list_of_circles.add(new Circle(width*2/3, height*1/5));
                println("remaining objects: " + list_of_circles.size() + list_of_lines.size());
                break;

        case 's':
                list_of_lines.add(new Line(width*2/3));
                println("remaining objects: " + list_of_circles.size() + list_of_lines.size());
                for (Circle circle : list_of_circles)
                {
                        circle.stopped = true;
                }
                break;

        case 'd':
                break;

        case 'e':

                break;
        }
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
    String[] appletArgs = new String[] { "drawms" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
