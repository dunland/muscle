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
    static int step;

    static std::vector<int> notes;
    static int note_idx;        // points at active (bass-)note
    static int note_change_pos; // defines at what position to increase note_idx

    static boolean setup; // when true, current score_step's setup function is executed.

    // SETUP etc:
    static void add_bassNote(int note); // adds a NOTE to notes[]

    // MODES:
    static void continuousBassNotes(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI);                     // initiates a continuous bass note from score
    static void continuousBassNote(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI);                      // play note only once (turn on never off):
    static void envelope_cutoff(Synthesizer *synth, TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI); // creates an envelope for cutoff filter via topography
    static void envelope_volume(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth); // creates an envelope for volume filter via topography
    static void crazyDelays(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth);     // changes the delay times on each 16th-step
    // static void set_ramp( midi::MidiInterface<HardwareSerial> MIDI, CC_Type cc_type, MIDI_Instrument midi_instr, int start_value, int end_value, int duration);

    // TODO: automatically assign first random note upon instantiation
    Score()
    {
        // notes.push_back(random(48, 60));
        //     Globals::print_to_console("note seed for score = ");
        //     Globals::print_to_console(notes[0]);
    }

    static TOPOGRAPHY beat_sum; // for TopographyLog
};