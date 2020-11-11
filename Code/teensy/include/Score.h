#pragma once

#include <Globals.h>
#include <vector>
#include <Instruments.h>

class Score
{
public:
    int step = 1;

    std::vector<int> notes;
    int note_idx = 0;      // points at active (bass-)note
    int note_iterator = 0; // defines at what position to increase note_idx

    // SETUP etc:
    void add_bassNote(int note, int note_iterator_); // adds a NOTE to notes[] and change when to change note in beat

    // MODES:
    void continuousBassNote(midi::MidiInterface<HardwareSerial> MIDI, int note_length);     // initiates a continuous bass note from score
    void envelope_cutoff(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI); // creates an envelope for cutoff filter via topography
    void envelope_volume(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI); // creates an envelope for volume filter via topography
    void crazyDelays(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI);     // changes the delay times on each 16th-step

    // TODO: automatically assign first random note upon instantiation
    Score()
    {
        // notes.push_back(random(48, 60));
        //     Globals::print_to_console("note seed for score = ");
        //     Globals::print_to_console(notes[0]);
    }

    static TOPOGRAPHY beat_sum; // for TopographyLog
};