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
Instrument *NanoKontrol::instrument = Drumset::instruments[0];
int NanoKontrol::paramsToChange[4] = {NanoKontrol::SET_INSTRUMENT, NanoKontrol::SET_CC_TYPE, NanoKontrol::SET_CC_MIN, NanoKontrol::SET_CC_MAX};
int NanoKontrol::incomingChannel = 0;
int NanoKontrol::incomingValue = 0;

const CC_Type dd200_controlValues[4] = {
    dd200_DelayDepth,
    dd200_DelayLevel,
    dd200_DelayTime,
    dd200_OnOff
    };

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
    strcpy(address1, strtokIndx);        // copy it to address1

    strtokIndx = strtok(NULL, "/"); // this continues where the previous call left off
    strcpy(address2, strtokIndx);

    strtokIndx = strtok(NULL, "/");
    incomingInt = atoi(strtokIndx); // convert this part to an integer

    std::string str = strtokIndx;

    if (strcmp("channel", address2) == 0) incomingChannel = incomingInt;
    else if (strcmp("value", address2) == 0) incomingValue = incomingInt;

    // instrument = Drumset::instruments[int(float(incomingInt) / 127.0 * (Drumset::instruments.size() - 1))];
}

// --------------------------------------------------------------------

void NanoKontrol::allocateData()
{
    // Serial.print("address 1: ");
    // Serial.println(address1);
    // Serial.print("address 2: ");
    // Serial.println(address2);
    // Serial.print("value: ");
    // Serial.println(incomingInt);

    // int *menu[4][1][1]; //[pos][address][value]
    // *menu[0] = incomingInt;

    std::vector<Instrument *> instruments = Drumset::instruments; // shorter variable name

    switch (incomingChannel)
    {
    case 0: // Fader 1: select instrument
    {
        instrument = instruments[int(float(incomingInt) / 127.0 * (instruments.size() - 1))];
    }
    break;

    case 17: // Knob 2: CC_Type or MIDI destination
    {
        paramsToChange[1] = (incomingValue >= 0) ? SET_CC_TYPE : SET_DEST; // always set cc_type.. TODO: implement destination setting and allow all according CC_Types (below)!
    }
    break;

    case 1: // Fader 2: set CC_Type
    {       // int val = -1; // results in "None"
        // switch(static_cast<CC_Type>(incomingValue))
        // {
        //     case CC_Type::Amplevel: instrument->midi_settings.cc_chan = Amplevel; break;
        //     case CC_Type::Attack: instrument->midi_settings.cc_chan = Attack; break;
        //     case CC_Type::Cutoff: instrument->midi_settings.cc_chan = Cutoff; break;
        //     case CC_Type::dd200_DelayDepth: instrument->midi_settings.cc_chan = dd200_DelayDepth; break;
        //     // ...
        // }
        instrument->midi_settings.cc_chan = dd200_controlValues[int(float(incomingValue) / 127.0 * 4)];
    }
    break;

    case 18: // Knob 3: set type
        paramsToChange[2] = (incomingValue < 64) ? SET_CC_MIN : SET_INCREASE;
        break;

    case 2: // Fader 3: set value
        switch (paramsToChange[2])
        {
        case SET_CC_MIN:
            instrument->midi_settings.cc_min = incomingValue;
            break;
        case SET_INCREASE:
            instrument->midi_settings.cc_increase_factor = map(float(incomingValue), 0.0, 127.0, -2.0, 2.0);
            break;
        default:
            break;
        }

    case 19: // Knob 4: set type
        paramsToChange[3] = (incomingValue < 64) ? SET_CC_MAX : SET_DECREASE;
        break;

    case 3: // Fader 4 : set value
        switch (paramsToChange[3])
        {
        case SET_CC_MAX:
            instrument->midi_settings.cc_max = incomingValue;
            break;
        case SET_DECREASE:
            instrument->midi_settings.cc_tidyUp_factor = map(float(incomingValue), 0.0, 127.0, -2.0, 2.0);
            break;

        default:
            break;
        }

    default:
        break;
    }

    // menu[0][0] = istr_idx;

    // menu[1][0] = cc_type;

    // menu[2][0] = cc_min;
    // menu[2][1] = increase;

    // menu[3][0] = cc_max;
    // menu[3][1] = decrease;

    // // instrument                          param    value
    // instruments(menu[0][0])->midi_settings.cc_chan = menu[0][1];
    // instruments(menu[0][0])->midi_settings.cc_min = menu[2][0];
    // instruments(menu[0][0])->midi_settings.cc_min = menu[2][0];
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
        allocateData();
        printToLCD();
        newData = false;
    }
}

// --------------------------------------------------------------------

void NanoKontrol::printToLCD()
{
    Hardware::lcd->setCursor(0, 0);
    Hardware::lcd->print("INT");

    Hardware::lcd->setCursor(4, 0);
    if (paramsToChange[1] == SET_CC_TYPE)
        Hardware::lcd->print("CC");
    else if (paramsToChange[1] == SET_DEST)
        Hardware::lcd->print("DST");

    Hardware::lcd->setCursor(8, 0);
    if (paramsToChange[2] == SET_CC_MIN)
        Hardware::lcd->print("min");
    else if (paramsToChange[2] == SET_INCREASE)
        Hardware::lcd->print("↑↑↑");

    Hardware::lcd->setCursor(12, 0);
    if (paramsToChange[3] == SET_CC_MAX)
        Hardware::lcd->print("max");
    else if (paramsToChange[3] == SET_DECREASE)
        Hardware::lcd->print("↓↓↓");

    Hardware::lcd->setCursor(0, 1);
    Hardware::lcd->print(Globals::DrumtypeToHumanreadable(instrument->drumtype));

    Hardware::lcd->setCursor(4, 1);
    if (paramsToChange[1] == SET_CC_TYPE)
        Hardware::lcd->print(instrument->midi_settings.cc_chan); // TODO: make human readable!
    else if (paramsToChange[1] == SET_DEST)
        Hardware::lcd->print("DST?");

    Hardware::lcd->setCursor(8, 1);
    if (paramsToChange[2] == SET_CC_MIN)
        Hardware::lcd->print(instrument->midi_settings.cc_min);
    else if (paramsToChange[2] == SET_INCREASE)
        Hardware::lcd->print(instrument->midi_settings.cc_increase_factor);

    Hardware::lcd->setCursor(12, 1);
    if (paramsToChange[3] == SET_CC_MAX)
        Hardware::lcd->print(Hardware::lcd->print(instrument->midi_settings.cc_max));
    else if (paramsToChange[3] == SET_DECREASE)
        Hardware::lcd->print(Hardware::lcd->print(instrument->midi_settings.cc_tidyUp_factor));

    // ----------------------- print to Serial ------------------------

    Serial.print("INT\t");
    if (paramsToChange[1] == SET_CC_TYPE)
        Serial.print("CC\t");
    else if (paramsToChange[1] == SET_DEST)
        Serial.print("DST\t");
    if (paramsToChange[2] == SET_CC_MIN)
        Serial.print("min\t");
    else if (paramsToChange[2] == SET_INCREASE)
        Serial.print("↑↑↑\t");
    if (paramsToChange[3] == SET_CC_MAX)
        Serial.print("max\t");
    else if (paramsToChange[3] == SET_DECREASE)
        Serial.print("↓↓↓\t");

    Serial.println();

    Serial.print(Globals::DrumtypeToHumanreadable(instrument->drumtype));
    Serial.print("\t");

    if (paramsToChange[1] == SET_CC_TYPE)
        Serial.print(instrument->midi_settings.cc_chan); // TODO: make human readable!
    else if (paramsToChange[1] == SET_DEST)
        Serial.print("DST?");
    Serial.print("\t");

    if (paramsToChange[2] == SET_CC_MIN)
        Serial.print(instrument->midi_settings.cc_min);
    else if (paramsToChange[2] == SET_INCREASE)
        Serial.print(instrument->midi_settings.cc_increase_factor);
    Serial.print("\t");

    if (paramsToChange[3] == SET_CC_MAX)
        Serial.print(instrument->midi_settings.cc_max);
    else if (paramsToChange[3] == SET_DECREASE)
        Serial.print(instrument->midi_settings.cc_tidyUp_factor);
    Serial.print("\t");

    Serial.println();
}