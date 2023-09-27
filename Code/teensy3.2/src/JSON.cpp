#include <JSON.h>
#include <Arduino.h>
#include <vector>
#include <MIDI.h>
// #include <Tsunami.h>
#include <ArduinoJson.h>
#include <Globals.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Song.h>
#include <SD.h>

// sends JSON as Serial information over port "Serial" (via USB)
void JSON::compose_and_send_json(std::vector<Instrument *> instruments)
{
    Song *active_song = Globals::active_song;
    size_t capacity = JSON_ARRAY_SIZE(active_song->notes.size()) + 9 * JSON_ARRAY_SIZE(16) + 9 * JSON_OBJECT_SIZE(8) + 2 * JSON_OBJECT_SIZE(9);
    DynamicJsonDocument doc(capacity);

    // ---------------------- Global values ---------------------------
    JsonObject score = doc.createNestedObject("score");
    score["millis"] = millis();
    score["current_beat_pos"] = Globals::current_beat_pos;
    score["score_step"] = active_song->step;

    // -------------------------- Score -------------------------------
    JsonArray score_notes = score.createNestedArray("notes");
    for (uint8_t i = 0; i < active_song->notes.size(); i++)
    {
        score_notes.add(active_song->notes[i]);
    }
    score["note"] = active_song->notes[active_song->note_idx];

    // Score topographies:
    JsonArray score_topo = score.createNestedArray("topo");
    for (uint8_t i = 0; i < active_song->beat_sum.a_16.size(); i++)
    {
        score_topo.add(active_song->beat_sum.a_16[i]);
    }

    // Score topography values:
    score["topo_ready"] = active_song->beat_sum.ready();                  // TODO: replace with beat_regularity at some point
    score["average_smooth"] = active_song->beat_sum.average_smooth;       // TODO: replace with beat_regularity at some point
    score["activation_thresh"] = active_song->beat_sum.activation_thresh; // TODO: replace with beat_regularity at some point

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
        instr["cc_val"] = instrument->midi.cc_val;
        instr["cc_increase"] = instrument->midi.cc_increase_factor;
        instr["cc_decay"] = instrument->midi.cc_tidyUp_factor;
        instr["effect"] = Globals::EffectstypeToHumanReadable(instrument->effect);
    }

    // transmit information:
    serializeJson(doc, Serial);
    Serial.println("");
}

// --------- write instrument sensitivity data to file on SD: --------
void JSON::save_settings_to_SD(std::vector<Instrument *> instruments)
{
    StaticJsonDocument<512> doc;

    for (auto &instrument : instruments)
    {
        JsonObject instr = doc.createNestedObject(Globals::DrumtypeToHumanreadable(instrument->drumtype));
        instr["drumtype"] = Globals::DrumtypeToHumanreadable(instrument->drumtype);
        instr["threshold"] = instrument->sensitivity.threshold;
        instr["crossings"] = instrument->sensitivity.crossings;
        instr["delayAfterStroke"] = instrument->sensitivity.delayAfterStroke;
        instr["countAfterFirstStroke"] = instrument->timing.countAfterFirstStroke;
    }

    Serial.print("Initializing SD card...");
    if (!SD.begin(BUILTIN_SDCARD))
    {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");

    // (WORKAROUND) remove existing file, otherwise data will be appended and cannot be read
    if (SD.exists("sense.txt"))
        SD.remove("sense.txt");

    File file = SD.open("sense.txt", FILE_WRITE); // seems to only work whith short file names
    // TODO: overwrite instead of append data
    if (file)
    {
        serializeJson(doc, file);
        file.close();
        Devtools::println_to_console("sensitivity data was saved to SD card.");
    }
    else
    {
        Devtools::println_to_console("error writing sensitivity data to SD card.");
    }
}

// ----------- read instrument sentivity data from SD card: -----------
String inString;
bool JSON::read_sensitivity_data_from_SD(std::vector<Instrument *> instruments)
{

    Serial.print("Initializing SD card...");
    if (!SD.begin(BUILTIN_SDCARD))
    {
        Serial.println("initialization failed!");
        return 1;
    }
    Serial.println("initialization done.");
    File file = SD.open("sense.txt", FILE_READ);

    if (!file)
    {
        Devtools::println_to_console("error opening sense.txt from SD card to read sensitivity data. Overwriting with data from settings.h...");
        save_settings_to_SD(Drumset::instruments);
        return 1;
    }
    if (file)
    {
        while (file.available())
        {
            inString += file.readString();
        }
        file.close();
        StaticJsonDocument<512> doc;
        deserializeJson(doc, inString);

        for (auto &instrument : instruments)
        {
            instrument->sensitivity.threshold = doc[Globals::DrumtypeToHumanreadable(instrument->drumtype)]["threshold"];
            instrument->sensitivity.crossings = doc[Globals::DrumtypeToHumanreadable(instrument->drumtype)]["crossings"];
            instrument->sensitivity.delayAfterStroke = doc[Globals::DrumtypeToHumanreadable(instrument->drumtype)]["delayAfterStroke"];
            instrument->timing.countAfterFirstStroke = doc[Globals::DrumtypeToHumanreadable(instrument->drumtype)]["countAfterFirstStroke"];
        }

        Devtools::println_to_console("reading sensitivity data from SD card complete.");
    }

    return 0;
}