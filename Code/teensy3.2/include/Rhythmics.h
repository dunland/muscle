#pragma once

#include <Globals.h>
#include <Serial.h>
#include <Instruments.h>
#include <Song.h>
#include <MIDI.h>

class Rhythmics{

    public:
    Rhythmics(){}

    void run_beat(int last_beat_pos, std::vector<Instrument *> instruments, midi::MidiInterface<HardwareSerial>); // goes step-wise through the beat, running a function at each step (when assigned)

    void set_beat(int beat_divisor, String function); // assign a function to a specific beat-step
};