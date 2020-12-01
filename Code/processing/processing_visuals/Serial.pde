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
                String inBuffer = myPort.readStringUntil(10); // parsing at "\n"

                if (inBuffer != null)
                {
                        if (str(inBuffer.charAt(0)).equals("{")) // message is JSON
                        {

                                JSONObject json;

                                // string to JSON object:
                                try{
                                        json = parseJSONObject(inBuffer);
                                } catch (Exception e) {
                                        println("parsing JSON object: " + e);
                                        json = null;
                                }

                                if (json != null)
                                {
                                        parseJSON(json);
                                }
                                // }
                                // WORKING..
                                // else // message is not JSON
// {
//         if (!json_was_initialized) json_was_initialized = true; // tell program that json can be used
//
//         print("→ treating incoming message as String:");
//         println(inBuffer);
//
//         inBuffer = myPort.readStringUntil(10); // 10 stands for \n → use Serial.println in MCU!
//
//         println("non-JSON-message is: " + inBuffer);
//
//         if (inBuffer != null) {
//                 inBuffer = inBuffer.substring(0, inBuffer.length()-2); // ATTENTION: could be different in teensy
//                 parse_incoming_string(inBuffer);
//         }
// }
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

                snare.json = json.getJSONObject("Snare");
                kick.json = json.getJSONObject("Kick");
                hihat.json = json.getJSONObject("Hihat");
                crash1.json = json.getJSONObject("Crash1");
                ride.json = json.getJSONObject("Ride");
                standtom1.json = json.getJSONObject("S_Tom1");
                tom2.json = json.getJSONObject("Tom2");
                cowbell.json = json.getJSONObject("Cowbell");

                parseScore(score.json);

                for (Instrument instrument : list_of_instruments)
                {
                        try {
                                instrument.parseJSON();
                        } catch(Exception e) {
                                println("trying to read from json file: " + e);
                        }
                }
        }
}

void parseJSON(JSONObject json)
{
        println("parsing!");
        score.json = json.getJSONObject("score");

        snare.json = json.getJSONObject("Snare");
        kick.json = json.getJSONObject("Kick");
        hihat.json = json.getJSONObject("Hihat");
        crash1.json = json.getJSONObject("Crash1");
        ride.json = json.getJSONObject("Ride");
        standtom1.json = json.getJSONObject("S_Tom1");
        tom2.json = json.getJSONObject("Tom2");
        cowbell.json = json.getJSONObject("Cowbell");

        parseScore(score.json);

        for (Instrument instrument : list_of_instruments)
        {
                try {
                        instrument.parseJSON();
                } catch(Exception e) {
                        println("trying to parse JSON for " + instrument.title + ": " + e);
                }
        }

        if (!json_was_initialized) json_was_initialized = true; // tell program that json can be used

}

void parseScore(JSONObject json)
{
        print("parsing score...");
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
        println("done!");
}

void parse_incoming_string(String message)
{
        if (message.equals("Snare")) snare.record_String("※", (height-20-600)/4); // Snaredrum
        if (message.equals("Kick")) kick.record_String("■", (height-20-600)/4); // Kickdrum
        if (message.equals("Cowbell")) cowbell.record_String("▲", (height-20-600)/4); // Cowbell
        if (message.equals("S_Tom1")) standtom1.record_String("□", (height-20-600)/4); // Standtom1
        // if (message.equals("O") standtom2.record_String("O", (height-20-600)/4); // Standtom2
        if (message.equals("Hihat")) hihat.record_String("x", (height-20-600)/4); // Hi-Hat
        // if (message.equals("°") tom1.record_String("°", (height-20-600)/4); // Tom 1
        if (message.equals("Tom2")) tom2.record_String("o", (height-20-600)/4); // Tom 2
        if (message.equals("Ride")) ride.record_String("xx", (height-20-600)/4); // Ride
        if (message.equals("Crash1")) crash1.record_String("-X-", (height-20-600)/4); // Crash1
        // if (message == "-XX-") crash2.record_String("-XX-", height/10); // Crash2
}
