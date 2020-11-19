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
int n = 0;

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
                instr.cc_plot.set_ticks(200,127);
                instr.cc_plot.drawMode = instr.plot.LINES;
                instr.cc_plot.draw_graphs = false;
                instr.cc_plot.pointsColor = color(i * (255/list_of_instruments.length), 100, 100);

        }

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
                textSize(20);
                colorMode(HSB);
                fill(instr.cc_plot.pointsColor);
                text("ø:\t" + instr.average_smooth
                     + "/" + instr.activation_thresh
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

// ------------------------ KEYS ---------------------------
void keyPressed()
{
        switch (key) {
        case 'a':
                list_of_circles.add(new Circle(width*2/3, height*1/5));
                println("a pressed.");
                //println("remaining objects: " + (list_of_circles.size() + list_of_lines.size()));
                break;

        case 's':
                list_of_lines.add(new Line(width*2/3));
                println("s pressed.");
                //println("remaining objects: " + (list_of_circles.size() + list_of_lines.size()));
                for (Circle circle : list_of_circles)
                {
                        circle.stopped = true;
                }
                break;

        case 'd':
                println("d pressed.");
                break;

        case 'e':
                println("e pressed.");

                break;
        }
}

void serialEvent(Serial myPort)
{
        if (serial_available) // using Serial at all
        {
                if (myPort.available() > 0)
                {
                        String inBuffer = myPort.readString();

                        // string to JSON object:
                        JSONObject json = parseJSONObject(inBuffer);
                        if (json == null) println("JSONObject could not be parsed");
                        else
                        {
                                score.json = json.getJSONObject("score");

                                snare.json = json.getJSONObject("snare");
                                kick.json = json.getJSONObject("kick");
                                hihat.json = json.getJSONObject("hihat");
                                crash1.json = json.getJSONObject("crash1");
                                ride.json = json.getJSONObject("ride");
                                standtom1.json = json.getJSONObject("standtom1");
                                tom2.json = json.getJSONObject("tom2");
                                cowbell.json = json.getJSONObject("cowbell");

                                parseScore(score.json);

                                for (Instrument instrument : list_of_instruments)
                                {
                                        try {
                                                parseInstrument(instrument);
                                        } catch(Exception e) {
                                                println(e);
                                        }
                                }
                        }
                }
        }

        else // instead read from file
        {
                JSONObject json = loadJSONObject("test.json");
                if (json == null) println("JSONObject could not be parsed");
                else
                {
                        score.json = json.getJSONObject("score");

                        snare.json = json.getJSONObject("snare");
                        kick.json = json.getJSONObject("kick");
                        hihat.json = json.getJSONObject("hihat");
                        crash1.json = json.getJSONObject("crash1");
                        ride.json = json.getJSONObject("ride");
                        standtom1.json = json.getJSONObject("standtom1");
                        tom2.json = json.getJSONObject("tom2");
                        cowbell.json = json.getJSONObject("cowbell");

                        parseScore(score.json);

                        for (Instrument instrument : list_of_instruments)
                        {
                                try {
                                        parseInstrument(instrument);
                                } catch(Exception e) {
                                        println(e);
                                }
                        }
                }
        }
}

void parseScore(JSONObject json)
{
        score.elapsedMillis = json.getInt("millis");
        score.current_beat_pos = json.getInt("current_beat_pos");
        score.step = json.getInt("score_step");
        score.note = json.getInt("note");
        score.average_smooth = json.getInt("average_smooth");
        score.activation_thresh = json.getInt("activation_thresh");

        // notes:
        JSONArray notes = json.getJSONArray("notes");
        for (int i = 0; i<notes.size(); i++)
        {
                if (score.notes.size()>i)
                        score.notes.set(i, notes.getInt(i));
        }

        // topography:
        JSONArray topo = json.getJSONArray("topo");
        for (int i = 0; i<topo.size(); i++)
        {
                if (score.topo.size()>i)
                        score.topo.set(i, topo.getInt(i));
                else score.topo.add(topo.getInt(i));
        }

}

void parseInstrument(Instrument instrument)
{


        instrument.average_smooth = instrument.json.getInt("average_smooth");
        instrument.activation_thresh = instrument.json.getInt("activation_thresh");
        instrument.cc_val = instrument.json.getInt("cc_val");
        instrument.cc_increase = instrument.json.getFloat("cc_increase");
        instrument.cc_decay = instrument.json.getFloat("cc_decay");


        // topography:
        JSONArray topo = instrument.json.getJSONArray("topo");
        for (int i = 0; i<topo.size(); i++)
        {
                if (instrument.topo.size()>i)
                        instrument.topo.set(i, topo.getInt(i));
                else instrument.topo.add(topo.getInt(i));
        }

}
