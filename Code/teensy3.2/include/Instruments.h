#pragma once

#include <Arduino.h>
#include <vector>
#include <Globals.h>
// #include <Tsunami.h>

#include <Song.h>
#include <Topography.h>

class Synthesizer;

class Instrument
{

public:
    Instrument(int pin_, DrumType drumtype_, std::vector<int> note_input = {int(random(21, 128))})
    {
        pin = pin_;
        drumtype = drumtype_;
        // allocateNotesToTarget(midiTargets.at(0), note_input); // assigns one random note so the array is not empty if not provided otherwise in input parameters
    }

    int pin;
    int led = LED_BUILTIN;

    EffectsType effect = Monitor;
    EffectsType lastEffect = Monitor; // used to store original effect temporarily (in footswitch functions)
    DrumType drumtype;

    // sensitivity and instrument calibration
    struct SENSITIVITY
    {
        int threshold;
        int crossings;
        int noiseFloor;
        int delayAfterStroke = 10; // TODO: assess best timing for each instrument
    } sensitivity;

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
        boolean wasHit = false;
        unsigned long lastHit = 0;

    } timing;

    struct MIDI_TARGET
    {
        std::vector<int> notes;
        int active_note;
        CC_Type cc_type;
        int random_cc_chan = 0; // integer standing for CC_Type in Random_CC_Effects
        float cc_val = 0;
        int cc_max = 127;              // MIDI values cannot be greater than this
        int cc_min = 30;               // MIDI values cannot be smaller than this
        int cc_standard = 30;          // a standard value to fall back to before changing Random_CC_Effect
        float cc_increase_factor = 1;  // factor by which MIDI vals will be increased upon hit
        float cc_tidyUp_factor = -0.1; // factor by which MIDI vals decay/increase each loop
        Synthesizer *synth;            // associated midi-instrument to address

    };

    std::vector<MIDI_TARGET*> midiTargets;

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
        int tsunami_track; // tracks will be allocated in tsunami_beat_playback
        int tsunami_channel = 0;

        boolean ready_to_shuffle = false; // a flag for Random_CC_Effect: resets midi.cc_type to random (once) when true

    } score;

    String output_string;

    TOPOGRAPHY topography;
    TOPOGRAPHY regularity;

    void trigger();

    void perform(std::vector<Instrument *> instruments);

    void tidyUp(); // turn of MIDI notes etc

    bool stroke_detected();

    ////////////////////// SETUP FUNCTIONS ////////////////////////////

    // set MIDI notes according to a vector list:
    void allocateNotesToTarget(MIDI_TARGET* midiTarget, std::vector<int> list);

    void addMidiTarget(CC_Type cc_type, Synthesizer *synth, int cc_max, int cc_min, float cc_increase_factor, float cc_tidyUp_factor);

    void addMidiTarget(CC_Type cc_type, Synthesizer *synth); // setup midi without CC params

    void setMidiTarget(MIDI_TARGET*, CC_Type cc_type, Synthesizer *synth, int cc_max, int cc_min, float cc_increase_factor, float cc_tidyUp_factor);

    // TODO: setup midi for ADSR Reflex effect!)

    void setup_sensitivity(int threshold_, int crossings_, int delayAfterStroke_, boolean firstStroke_);

    void calculateNoiseFloor();

    void setInstrumentPrintString();

    void set_effect(EffectsType effect_); // just set effect

    // void smoothen_dataArray(Instrument *instrument);

    // trigger effects: -----------------------------------------------

    void playMidi();

    void monitor(); // just prints what is being played.

    void toggleRhythmSlot();

    void footswitch_recordSlots();

    void getTapTempo();

    void change_cc_in(); // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.

    void reflex_and_playMidi(); // plays note when hit and changes CC

    void random_change_cc_in(); // changes CC using integer, not CC_Type.

    void swell_rec();

    void countup_topography();

    void mainNoteIteration(Synthesizer *); // increase note_index of Score and play it

    // timed events: --------------------------------------------------

    void swell_perform();

    void sendMidiNotes_timed();

    void setInstrumentSlots(); // for Footswitchlooper

    void topography_midi_effects(std::vector<Instrument *> instruments); // MIDI playback according to beat_topography

    void tsunamiLink(); // Tsunami beat-linked pattern

    // final tidy up functions: ---------------------------------------

    void turnMidiNoteOff();

    void change_cc_out();

    void shuffle_cc(MIDI_TARGET*, boolean force_);
};

class Drumset
{
public:
    static Instrument *snare;
    static Instrument *hihat;
    static Instrument *kick;
    static Instrument *tom1;
    static Instrument *tom2;
    static Instrument *standtom;
    static Instrument *crash1;
    static Instrument *ride;
    static Instrument *crash2;

    static std::vector<Instrument *> instruments; // all instruments go in here
};

// TODO: REORGANIZE SYNTHESIZERS CLASS:
// put "synths" into "Instruments"?, delete "Synthesizers"
class Synthesizers
{
    public:
    static Synthesizer *mKorg; // create a KORG microKorg instrument called mKorg
    static Synthesizer *volca; // create a KORG Volca Keys instrument called volca
    static Synthesizer *dd200;
    static Synthesizer *whammy;
    static Synthesizer *kaossPad3;

    static std::vector<Synthesizer*> synths;
};