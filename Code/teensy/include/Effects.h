#pragma once

#include <MIDI.h>
#include <Instruments.h>
#include <vector>

class Instrument;

class Effect
{
public:
    // trigger effects: -----------------------------------------------
    static void playMidi_rawPin(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI); // instead of stroke detection, MIDI notes are sent directly when sensitivity threshold is crossed. may sound nice on cymbals..

    static void cc_effect_rawPin(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI); // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.

    static void playMidi(Instrument *instrument, midi::MidiInterface<HardwareSerial>);

    static void monitor(Instrument *instrument); // just prints what is being played.

    static void toggleRhythmSlot(Instrument *instrument);

    static void footswitch_recordSlots(Instrument *instrument);

    static void getTapTempo();

    static void swell_rec(Instrument *instrument, midi::MidiInterface<HardwareSerial>);

    static void countup_topography(Instrument *instrument);

    static void tsunamiLink(Instrument *instrument); // Tsunami beat-linked pattern

    // timed events: --------------------------------------------------

    static void swell_perform(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI);

    static void sendMidiNotes_timed(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI);

    static void setInstrumentSlots(Instrument *instrument); // for Footswitchlooper

    static void tsunami_beat_playback(Instrument *instrument);

    static TOPOGRAPHY beat_sum; // for TopographyLog

    static void topography_midi_effects(Instrument *instrument, Instrument *instruments[Globals::numInputs], midi::MidiInterface<HardwareSerial>); // MIDI playback according to beat_topography

    // final tidy up functions: ---------------------------------------

    static void turnMidiNoteOff(Instrument *instrument, midi::MidiInterface<HardwareSerial>);

    static void decay_ccVal(Instrument *instrument, midi::MidiInterface<HardwareSerial>);
};
