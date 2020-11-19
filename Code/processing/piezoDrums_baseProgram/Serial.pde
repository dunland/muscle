
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
        score.ready = (json.getInt("topo_ready") == 1);

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
