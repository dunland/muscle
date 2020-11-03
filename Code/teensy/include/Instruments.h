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
        unsigned long last_notePlayed;
        boolean read_rhythm_slot[8];
        boolean set_rhythm_slot[8];

        // swell-effect:
        int swell_val = 10;
        int swell_factor = 2;      // factor by which to increase swell_val
        int swell_state = 1;       // waits for first tap
        // int min_swell_val = 10;    // the minimum the swell value can be
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
        std::vector<int> notes;
        int active_note;
        int cc_chan;
    } midi;

    struct TIMING
    {
        volatile unsigned long lastPinActiveTime;
        volatile unsigned long firstPinActiveTime;
        volatile int counts;
        boolean stroke_flag = false;
    } timing;

    TOPOGRAPHY topography;

    void trigger(Instrument *, midi::MidiInterface<HardwareSerial>);

    void perform(Instrument *, Instrument *instruments[Globals::numInputs], midi::MidiInterface<HardwareSerial>);

    void tidyUp(Instrument *, midi::MidiInterface<HardwareSerial>); // turn of MIDI notes etc

    bool stroke_detected(Instrument *instrument);

    void setup_notes(std::vector<int> list);

    void calculateNoiseFloor(Instrument *);

    // void smoothen_dataArray(Instrument *instrument);
};
