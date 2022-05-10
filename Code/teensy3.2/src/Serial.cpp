#include <Serial.h>
#include <Arduino.h>
#include <vector>
#include <MIDI.h>
// #include <Tsunami.h>
#include <ArduinoJson.h>
#include <Globals.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Score/Score.h>

char NanoKontrol::address1[NanoKontrol::numChars] = {0};
char NanoKontrol::address2[NanoKontrol::numChars] = {0};
int NanoKontrol::incomingInt = 0;
bool NanoKontrol::newData = false;
char NanoKontrol::receivedChars[numChars];
char NanoKontrol::tempChars[numChars]; // temporary array for use when parsing


// sends JSON as Serial information over port "Serial" (via USB)
void JSON::compose_and_send_json(std::vector<Instrument *> instruments)
{
    Score *active_score = Globals::active_score;
    size_t capacity = JSON_ARRAY_SIZE(active_score->notes.size()) + 9 * JSON_ARRAY_SIZE(16) + 9 * JSON_OBJECT_SIZE(8) + 2 * JSON_OBJECT_SIZE(9);
    DynamicJsonDocument doc(capacity);

    // ---------------------- Global values ---------------------------
    JsonObject score = doc.createNestedObject("score");
    score["millis"] = millis();
    score["current_beat_pos"] = Globals::current_beat_pos;
    score["score_step"] = active_score->step;

    // -------------------------- Score -------------------------------
    JsonArray score_notes = score.createNestedArray("notes");
    for (uint8_t i = 0; i < active_score->notes.size(); i++)
    {
        score_notes.add(active_score->notes[i]);
    }
    score["note"] = active_score->notes[active_score->note_idx];

    // Score topographies:
    JsonArray score_topo = score.createNestedArray("topo");
    for (uint8_t i = 0; i < active_score->beat_sum.a_16.size(); i++)
    {
        score_topo.add(active_score->beat_sum.a_16[i]);
    }

    // Score topography values:
    score["topo_ready"] = active_score->beat_sum.ready();                  // TODO: replace with beat_regularity at some point
    score["average_smooth"] = active_score->beat_sum.average_smooth;       // TODO: replace with beat_regularity at some point
    score["activation_thresh"] = active_score->beat_sum.activation_thresh; // TODO: replace with beat_regularity at some point

    // --------------------------- Instruments ------------------------
    for (auto &instrument : instruments)
    {
        JsonObject instr = doc.createNestedObject(Globals::DrumtypeToHumanreadable(instrument->drumtype));

        JsonArray topo = instr.createNestedArray("topo");

        // instrument topography:
        for (uint8_t i = 0; i < instrument->topography.a_16.size(); i++)
        {
            topo.add(instrument->topography.a_16[i]);
        }

        // topo information:
        instr["average_smooth"] = instrument->topography.average_smooth;
        instr["activation_thresh"] = instrument->topography.activation_thresh;

        // instrument values:
        instr["wasHit"] = instrument->timing.wasHit;
        instr["cc_val"] = instrument->midi_settings.cc_val;
        instr["cc_increase"] = instrument->midi_settings.cc_increase_factor;
        instr["cc_decay"] = instrument->midi_settings.cc_tidyUp_factor;
        instr["effect"] = Globals::EffectstypeToHumanReadable(instrument->effect);
    }

    // transmit information:
    serializeJson(doc, Serial);
    Serial.println("");
}

///////////////////////////// NANOKONTROL /////////////////////////////

void NanoKontrol::recvWithStartEndMarkers()
{
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '[';
    char endMarker = ']';
    char rc;

    while (Serial.available() > 0 && newData == false)
    {
        rc = Serial.read();

        if (recvInProgress == true)
        {
            if (rc != endMarker)
            {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars)
                {
                    ndx = numChars - 1;
                }
            }
            else
            {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker)
        {
            recvInProgress = true;
        }
    }
}

// --------------------------------------------------------------------

void NanoKontrol::parseData()
{ // split the data into its parts

    char *strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars, "/"); // get the first part - the string
    strcpy(address1, strtokIndx);        // copy it to messageFromPC

    strtokIndx = strtok(NULL, "/"); // this continues where the previous call left off
    strcpy(address2, strtokIndx);

    strtokIndx = strtok(NULL, "/");
    incomingInt = atoi(strtokIndx); // convert this part to an integer
}

// --------------------------------------------------------------------

void NanoKontrol::loop()
{
    recvWithStartEndMarkers();
    if (newData == true)
    {
        strcpy(tempChars, receivedChars);
        // this temporary copy is necessary to protect the original data
        //   because strtok() used in parseData() replaces the commas with \0
        parseData();
        showParsedData();
        newData = false;
    }
}

// --------------------------------------------------------------------

void NanoKontrol::showParsedData() {
  Serial.print("address 1: ");
  Serial.println(address1);
  Serial.print("address 2: ");
  Serial.println(address2);
  Serial.print("value: ");
  Serial.println(incomingInt);
}