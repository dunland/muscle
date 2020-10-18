#pragma once

#include <MIDI.h>
#include <Instruments.h>

class Instrument;

class Effect
{
public:
    
    // trigger effects: -----------------------------------------------

    static void playMidi(Instrument* instrument, midi::MidiInterface<HardwareSerial>);
    
    static void monitor(Instrument* instrument); // just prints what is being played.
    
    static void toggleRhythmSlot(Instrument* instrument);

    static void footswitch_recordSlots(Instrument* instrument);

    // timed events: --------------------------------------------------

    // static void swell_perform(i, instruments[i]->effect); // ...updates once a 32nd-beat-step


};
