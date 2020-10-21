#pragma once

#include <Arduino.h>
#include <vector>
#include <Globals.h>
#include <Effects.h>
#include <Tsunami.h>

class Instrument
{

public:
    Instrument()
    {
    }

    int pin;

    EffectsType effect;
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
<<<<<<< HEAD
        std::vector<int> notes;
        int active_note;
=======
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
        unsigned long last_notePlayed;
        boolean read_rhythm_slot[8];
        boolean set_rhythm_slot[8];

        // swell-effect:
        int swell_val = 10;
        int swell_state = 1;       // waits for first tap
        int num_of_swell_taps = 0; // will be used in both swell_rec() and swell_beat(). serves as swell_val for MIDI notes.
        int swell_stroke_interval; // will be needed for timed replay
        int swell_beatStep = 0;    // increases with beatCount and initiates action.
        unsigned long swell_beatPos_sum = 0;

        //field recordings
        int allocated_track; // tracks will be allocated in tsunami_beat_playback
        int allocated_channel = 0;
    } score;

    struct MIDI
    {
<<<<<<< HEAD
=======
        std::vector<int> notes;
        int active_note;
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
        int cc_chan;
    } midi;

    struct TIMING
    {
        volatile unsigned long lastPinActiveTime;
        volatile unsigned long firstPinActiveTime;
        volatile int counts;
    } timing;

    TOPOGRAPHY topography;

    void trigger(Instrument *, midi::MidiInterface<HardwareSerial>);

<<<<<<< HEAD
    void perform(Instrument *, midi::MidiInterface<HardwareSerial>);
=======
    void perform(Instrument *, Instrument *instruments[Globals::numInputs], midi::MidiInterface<HardwareSerial>);
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e

    void tidyUp(Instrument *, midi::MidiInterface<HardwareSerial>); // turn of MIDI notes etc

    bool stroke_detected(Instrument *instrument);

<<<<<<< HEAD
    void setup_notes(std::vector<int> list)
    {
        for (int i = 0; i < list.size(); i++)
        {
            score.notes[i] = list[i];
        }
    }
=======
    void setup_notes(std::vector<int> list);
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e

    void calculateNoiseFloor(Instrument *);

    void smoothen_dataArray(Instrument *instrument);
};
