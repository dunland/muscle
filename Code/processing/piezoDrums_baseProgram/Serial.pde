boolean json_was_initialized = false; // tells program whether json is ok to be used or not

void serialEvent(Serial myPort)
{
        if (serial_available) // using Serial at all
                read_serial_stream(myPort);

        else // instead read from file
                read_json_from_file();
}

void read_serial_stream(Serial myPort)
{
        if (myPort.available() > 0)
        {
                String inBuffer = myPort.readStringUntil(10); // ATTENTION: does this still work with JSON???

                if (inBuffer != null)
                {

                        if (str(inBuffer.charAt(0)) == "{") // message is JSON
                        {

                                if (inBuffer != null) print(inBuffer);
                                JSONObject json;

                                // string to JSON object:
                                try{
                                        json = parseJSONObject(inBuffer);
                                } catch (Exception e) {
                                        println(e);
                                        json = null;
                                }


                                if (json == null)
                                {
                                        println("JSON could not be parsed");
                                }
                                if (!json_was_initialized) json_was_initialized = true; // tell program that json can be used
                                parseJSON(json);
                        }
                        else // message is not JSON
                        {
                                // inBuffer = myPort.readStringUntil(10); // 10 stands for \n → use Serial.println in MCU!
                                println("→ treating incoming message as String");
                                parse_incoming_string(inBuffer); // message is probably not a json format. try treating as String
                        }

                }
        }
}

void read_json_from_file()
{
        JSONObject json = loadJSONObject("test.json");
        if (json == null) println("JSONObject could not be parsed");
        else
        {
                if (!json_was_initialized) json_was_initialized = true; // tell program that json can be used

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

void parseJSON(JSONObject json)
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

void parseScore(JSONObject json)
{
        score.elapsedMillis = json.getInt("millis");
        score.current_beat_pos = json.getInt("current_beat_pos");
        score.step = json.getInt("score_step");
        score.note = json.getInt("note");
        score.average_smooth = json.getInt("average_smooth");
        score.activation_thresh = json.getInt("activation_thresh");
        score.ready = (json.getBoolean("topo_ready"));

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
        instrument.cc_val = instrument.json.getFloat("cc_val");
        instrument.cc_increase = instrument.json.getFloat("cc_increase");
        instrument.cc_decay = instrument.json.getFloat("cc_decay");
        instrument.effect = instrument.json.getString("effect");


        // topography:
        JSONArray topo = instrument.json.getJSONArray("topo");
        for (int i = 0; i<topo.size(); i++)
        {
                if (instrument.topo.size()>i)
                        instrument.topo.set(i, topo.getInt(i));
                else instrument.topo.add(topo.getInt(i));
        }

}

void parse_incoming_string(String message)
{
        message = message.substring(0, message.length()-2); // ATTENTION: could be different in teensy

        if (message.equals("※")) snare.record_String("※", (height-20-600)/4); // Snaredrum
        if (message.equals("■")) kick.record_String("■", (height-20-600)/4); // Kickdrum
        if (message.equals("▲")) cowbell.record_String("▲", (height-20-600)/4); // Cowbell
        if (message.equals("□")) standtom1.record_String("□", (height-20-600)/4); // Standtom1
        // if (message.equals("O") standtom2.record_String("O", (height-20-600)/4); // Standtom2
        if (message.equals("x")) hihat.record_String("x", (height-20-600)/4); // Hi-Hat
        // if (message.equals("°") tom1.record_String("°", (height-20-600)/4); // Tom 1
        if (message.equals("o")) tom2.record_String("o", (height-20-600)/4); // Tom 2
        if (message.equals("xx")) ride.record_String("xx", (height-20-600)/4); // Ride
        if (message.equals("-X-")) crash1.record_String("-X-", (height-20-600)/4); // Crash1
        // if (message == "-XX-") crash2.record_String("-XX-", height/10); // Crash2
}
