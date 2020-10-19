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

    static void getTapTempo();

    static void swell_rec(Instrument* instrument, midi::MidiInterface<HardwareSerial>);



    // timed events: --------------------------------------------------

    static void swell_perform(Instrument* instrument, midi::MidiInterface<HardwareSerial> MIDI); // ...updates once a 32nd-beat-step


};
