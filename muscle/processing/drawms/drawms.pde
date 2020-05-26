//SIGNAL VARIABLES
ArrayList<Circle> list_of_circles = new ArrayList<Circle>();
ArrayList<Line> list_of_lines = new ArrayList<Line>();

int maxSignalExtent = 500;
int signalGrowthSpeed = 2;

void setup()
{
        size(800,600, P2D);
}

void draw()
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

void keyPressed()
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
