#pragma once

#include <Arduino.h>
#include <vector>
#include <Globals.h>
#include <Effects.h>
#include <Tsunami.h>

class Instrument
{

public:
    Instrument(int pin_, DrumType drumtype_)
    {
        pin = pin_;
        drumtype = drumtype_;
    }

    int pin;
    int led = LED_BUILTIN;

    EffectsType effect;
    EffectsType lastEffect; // used to store original effect temporarily (in footswitch functions)
    DrumType drumtype;

    String output_string;

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
        int swell_factor = 2; // factor by which to increase swell_val
        int swell_state = 1;  // waits for first tap
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
        float cc_val = 0;
        int cc_max = 127;               // MIDI values cannot be greater than this
        int cc_min = 30;                // MIDI values cannot be smaller than this
        float cc_increase_factor = 0.7; // factor by which MIDI vals will be increased upon hit
        float cc_decay_factor = 1;      // factor by which MIDI vals decay
    } midi;

    struct TIMING
    {
        volatile unsigned long lastPinActiveTime;
        volatile unsigned long firstPinActiveTime;
        unsigned long lastPinActiveTimeCopy;
        unsigned long firstPinActiveTimeCopy;
        boolean countAfterFirstStroke = false; // start counting after first threshold crossing or using delayAfterStroke
        volatile int counts = 0;
        boolean stroke_flag = false;
        int countsCopy;
    } timing;

    TOPOGRAPHY topography;

    void trigger(Instrument *, midi::MidiInterface<HardwareSerial>);

    void perform(Instrument *, std::vector<Instrument *> instruments, midi::MidiInterface<HardwareSerial>);

    void tidyUp(Instrument *, midi::MidiInterface<HardwareSerial>); // turn of MIDI notes etc

    bool stroke_detected();

    void setup_notes(std::vector<int> list);

    void setup_midi(CC_Type cc_type, int cc_max, int cc_min, float cc_increase_factor, float cc_decay_factor);

    void setup_sensitivity(int threshold_, int crossings_, int delayAfterStroke_, boolean firstStroke_);

    void calculateNoiseFloor(Instrument *);

    void setInstrumentPrintString();


    // void smoothen_dataArray(Instrument *instrument);
};
