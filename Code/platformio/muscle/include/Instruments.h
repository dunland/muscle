#pragma once

#include <Arduino.h>
#include <vector>
#include <Globals.h>
#include <Effects.h>

class Instrument
{

public:
    Instrument()
    {
    }

    EffectsType effect;
    EffectsType initialEffect; // holds the pinAction array as defined above
    DrumType drumtype;

    // sensitivity and instrument calibration
    struct SENSITIVITY
    {
        int threshold;
        int crossings;
        int noiseFloor;
        int delayAfterStroke = 10; // TODO: assess best timing for each instrument
    } sensitivity;

    struct SCORE
    {
        std::vector<int> notes;
        int active_note;
        unsigned long last_notePlayed;
        boolean read_rhythm_slot[8];
        boolean set_rhythm_slot[8];
    } score;

    TOPOGRAPHY topography;

    void trigger(Instrument *, midi::MidiInterface<HardwareSerial>);

    void perform(EffectsType effect);

    void setup_notes(std::vector<int> list)
    {
        for (int i = 0; i < list.size(); i++)
        {
            score.notes[i] = list[i];
        }
    }
};
