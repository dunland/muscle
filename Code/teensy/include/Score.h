#pragma once

#include <Globals.h>
#include <vector>
#include <Instruments.h>

class Score
{
public:
    std::vector<int> notes;

    TOPOGRAPHY beat_regularity;

    void continuousBassNote(midi::MidiInterface<HardwareSerial> MIDI);                      // initiates a continuous bass note from score
    void envelope_cutoff(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI); // creates an envelope for cutoff filter via topography
    void envelope_volume(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI); // creates an envelope for volume filter via topography
    void crazyDelays(midi::MidiInterface<HardwareSerial> MIDI);                             // changes the delay times on each 16th-step
    void regularHighNote(Instrument *instrument);                                           // plays a high note according to instrument pattern

    // TODO: automatically assign first random note upon instantiation
    Score()
    {
        // notes.push_back(random(48, 60));
    //     Globals::print_to_console("note seed for score = ");
    //     Globals::print_to_console(notes[0]);
    }
};