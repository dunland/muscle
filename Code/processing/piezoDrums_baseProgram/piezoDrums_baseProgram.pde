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
String printstr = "";
String print_topo = "";
String incoming_millis_str = "";
int[] topo = new int[16];
boolean collect_r = false; // enables data recording for "regularity" array
boolean collect_m = false; // enables storage of incoming millis

Plot plot = new Plot(0, 16, 0, 1, 320, 100);
Score score = new Score();
Instrument snare = new Instrument("SNARE");
Instrument kick = new Instrument("KICK");
Instrument hihat = new Instrument("HIHAT");
Instrument standtom1 = new Instrument("STANDTOM 1");
Instrument tom2 = new Instrument("TOM 2");
Instrument ride = new Instrument("RIDE");
Instrument cowbell = new Instrument("COWBELL");
Instrument crash1 = new Instrument("CRASH 1");

Instrument list_of_instruments[] = {snare, kick, hihat, standtom1, tom2, ride, cowbell, crash1};

Grid grid;


///////////////////////////////////////////////////////////////
////////////////////////// SETUP //////////////////////////////
///////////////////////////////////////////////////////////////


void setup()
{

        size(1600, 800);
        printArray(Serial.list());
        String portName = Serial.list()[0]; // find right Serial port from list
        try {
                myPort = new Serial(this, portName, 115200);
        } catch(Exception e) {
                println(e);
                serial_available = false;
        }

        plot.drawMode = plot.BARPLOT;
        plot.set_ticks(1, 3);
        plot.set_title("BEAT SUM");

        for (int i = 0; i<list_of_instruments.length; i++)
        {
                Instrument instr = list_of_instruments[i];

                // stroke plot:
                instr.plot.drawMode = instr.plot.BARPLOT;
                instr.plot.set_ticks(16,2);
                instr.plot.set_title(instr.title);
                colorMode(HSB);
                instr.plot.pointsColor = color(i * (255/list_of_instruments.length), 100, 100);

                // cc_plot:
                instr.cc_plot.set_ticks(200,1);
                instr.cc_plot.drawMode = instr.plot.LINES;
                instr.cc_plot.draw_title = false;
                instr.cc_plot.draw_labels = false;
                instr.cc_plot.draw_xticks = false;
                // instr.cc_plot.draw_yticks = false;
                instr.cc_plot.pointsColor = color(i * (255/list_of_instruments.length), 100, 100);

        }
        grid = new Grid(width/2, 0);
}

///////////////////////////////////////////////////////////////
////////////////////////// LOOP ///////////////////////////////
///////////////////////////////////////////////////////////////

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
                                println("remaining objects: " +  int(list_of_circles.size() + list_of_lines.size()));
                        }
                }
        }

        // initialize grid once:
        if (snare.topo.size() == 16 && grid.init)
        {
                grid.create_vertices(16, 16);
                grid.init = false;
        }

        // draw vertices:
        if (snare.topo.size() == 16) grid.draw();

        // ------------------------ draw plots ---------------------------
        int dist_left = 50;
        int dist_top = 30;
        plot.updateValues(score.topo, true);
        plot.draw(dist_left, 650);

        for (int i = 0; i<list_of_instruments.length; i++)
        {
                Instrument instr = list_of_instruments[i];

                instr.plot.updateValues(instr.topo, true);
                instr.plot.draw(dist_left, dist_top+ i* (height-200)/list_of_instruments.length);

                instr.cc_plot.draw(dist_left + 350, dist_top+ i* (height-200)/list_of_instruments.length);

                textAlign(LEFT,LEFT);
                textSize(16);
                colorMode(HSB);
                fill(instr.cc_plot.pointsColor);
                text("ø:\t" + instr.average_smooth
                     + "/" + instr.activation_thresh
                     + " " + instr.title
                     + "\t ― " + instr.effect
                     + "\nCC:\t" + instr.cc_val
                     + " | " + instr.cc_increase
                     + " | " + instr.cc_decay,
                     instr.plot.axis_width + instr.cc_plot.axis_width + dist_left*2, dist_top + i* (height-200)/list_of_instruments.length);

                instr.record_value(instr.cc_val, 200);
        }

        // ------------------------ Auxiliary Info -----------------------

        // mouse position and circles
        textAlign(RIGHT, BOTTOM);
        textSize(14);
        fill(255);
        text("circles: " + list_of_circles.size() + "\n" + mouseX + " " + mouseY, width, height);

        // millis:
        textAlign(LEFT, BOTTOM);
        text(score.elapsedMillis, 0, height);


        // ------------------------ Read Serial ---------------------------
        serialEvent(myPort);

        // ------------------------ print beat step -------------------
        textAlign(CENTER,BOTTOM);
        // int current_16th_step = current_16ths_step();
        if (score.current_beat_pos % 4 == 0)
        {
                textSize(24);
                fill(255,0,0);
        }
        else
        {
                textSize(16);
                fill(255);
        }
        text(str(score.current_beat_pos), width/2, height);

        // --------------------- print incoming millis -------------------
        textAlign(LEFT,BOTTOM);
        textSize(14);
        fill(255);
        text(incoming_millis_str, 0, height);

}
