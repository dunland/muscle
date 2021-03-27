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

    boolean setup = true; // when true, current score_step's setup function is executed.

    TOPOGRAPHY beat_sum;         // sum of all instrument topographies
    TOPOGRAPHY beat_regularity;  // for advance of step
    TOPOGRAPHY topo_midi_effect; // for TopographyMidiEffect

    // SETUP etc:
    void set_step_function(int trigger_step, Instrument *instrument, EffectsType); // TODO: set score-step-functions here
    void set_notes(std::vector<int> list);
    void add_bassNote(int note); // adds a NOTE to notes[]

    // STANDARD RUN: select according to score->name
    void run(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI);                                                                                                                                                                                                                                                           // iterates through all score steps, executing the current step functions
    void run_doubleSquirrel(midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *mKorg, Synthesizer *volca); // TODO: tentative, as this should be dynamic later..

    void run_elektrosmoff(Synthesizer* mKorg, midi::MidiInterface<HardwareSerial> MIDI);

    void run_experimental(Synthesizer *mKorg, Synthesizer *volca);

    // MODES:
    void playRhythmicNotes(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI, int note_change_pos_ = 0); // initiates a continuous bass note from score

    void playSingleNote(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI); // play note only once (turn on never off):

    void envelope_cutoff(Synthesizer *synth, TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI); // creates an envelope for cutoff filter via topography

    void envelope_volume(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth); // creates an envelope for volume filter via topography

    void crazyDelays(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth); // changes the delay times on each 16th-step

    // void set_ramp( midi::MidiInterface<HardwareSerial> MIDI, CC_Type cc_type, MIDI_Instrument midi_instr, int start_value, int end_value, int duration);
};