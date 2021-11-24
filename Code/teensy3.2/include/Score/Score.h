#pragma once

#include <Globals.h>
#include <vector>
// #include <Instruments.h>
#include <MIDI.h>

class Instrument;
class Synthesizer;

class Score
{
public:
    Score(String name_)
    {
        name = name_;
        // setup names of elements for display on Serial Monitor:
        beat_sum.tag = "v";
        beat_regularity.tag = "r";

        // add a bass note to Score
        notes.push_back(int(random(36, 48)));
        Globals::print_to_console("active_score->note[0] = ");
        Globals::println_to_console(notes[0]);
    }

    String name; // name to be displayed on LCD

    int step = 0;
    std::vector<int> steps;
    int note_idx = 0;        // points at active (bass-)note
    int note_change_pos = 0; // defines at what position to increase note_idx
    std::vector<int> notes;

    struct tempo
    {
        int min_tempo = 0;
        int max_tempo = 999;
        int tapTempoResetTime = 4000; // time to restart tapTempo if not used for this long
        unsigned int tapTempoTimeOut = 2000;   // do not count second tap, if time gap to first one exceeds this
    } tempo;

    // TODO: make this a functional bool and reset instruments etc when calling (and deactivate it automatically)!
    boolean setup = true; // when true, current score_step's setup function is executed.

    // TODO: move this to Rhythmics
    TOPOGRAPHY beat_sum;         // sum of all instrument topographies
    TOPOGRAPHY beat_regularity;  // for advance of step
    TOPOGRAPHY topo_midi_effect; // for TopographyMidiEffect

    // --------------------------- SETUP etc: -------------------------
    void set_step_function(int trigger_step, Instrument *instrument, EffectsType); // TODO: set score-step-functions here
    void set_notes(std::vector<int> list);
    void add_bassNote(int note); // adds a NOTE to notes[]
    void increase_step();
    void proceed_to_next_score();
    void setTempoRange(int min_tempo_, int max_tempo_);
    void resetInstruments();

    // ---------------------------- SONGS: ---------------------------
    // STANDARD RUN: select according to score->name
    void run(midi::MidiInterface<HardwareSerial> MIDI); // iterates through all score steps, executing the current step functions

    void run_sattelstein(midi::MidiInterface<HardwareSerial> MIDI);

    void run_monitoring(midi::MidiInterface<HardwareSerial> MIDI);

    void run_doubleSquirrel(midi::MidiInterface<HardwareSerial> MIDI); // TODO: tentative, as this should be dynamic later..

    void run_elektrosmoff(midi::MidiInterface<HardwareSerial> MIDI);

    void run_randomVoice(midi::MidiInterface<HardwareSerial> MIDI);

    void run_a72(midi::MidiInterface<HardwareSerial> MIDI);
    
    void run_control_dd200(midi::MidiInterface<HardwareSerial> MIDI);

    void run_whammyMountains(midi::MidiInterface<HardwareSerial> MIDI);

    // ------------------------------- MODES: (deprecated) ------------
    void playRhythmicNotes(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI, int note_change_pos_ = 0); // initiates a continuous bass note from score

    void playSingleNote(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI); // play note only once (turn on, never off)

    void playLastThreeNotes(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI);

    void envelope_cutoff(Synthesizer *synth, TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI); // creates an envelope for cutoff filter via topography

    void envelope_volume(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth); // creates an envelope for volume filter via topography

    void crazyDelays(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth); // changes the delay times on each 16th-step

    // void set_ramp( midi::MidiInterface<HardwareSerial> MIDI, CC_Type cc_type, MIDI_Instrument midi_instr, int start_value, int end_value, int duration);
};