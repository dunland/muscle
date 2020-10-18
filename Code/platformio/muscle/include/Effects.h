#pragma once

#include <MIDI.h>
#include <Instruments.h>

class Instrument;

class Effect
{
public:
    static void playMidi(Instrument* instrument, midi::MidiInterface<HardwareSerial>);
    static void monitor(Instrument* instrument); // just prints what is being played.
};
