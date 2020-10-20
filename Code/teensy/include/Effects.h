#pragma once

#include <MIDI.h>
#include <Instruments.h>
#include <vector>

class Instrument;

class Effect
{
public:
    // trigger effects: -----------------------------------------------

    static void playMidi(Instrument *instrument, midi::MidiInterface<HardwareSerial>);

    static void monitor(Instrument *instrument); // just prints what is being played.

    static void toggleRhythmSlot(Instrument *instrument);

    static void footswitch_recordSlots(Instrument *instrument);

    static void getTapTempo();

    static void swell_rec(Instrument *instrument, midi::MidiInterface<HardwareSerial>);

    static void tsunamiLink(Instrument *instrument); // Tsunami beat-linked pattern

    // timed events: --------------------------------------------------

    static void swell_perform(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI);

    static void sendMidiNotes_timed(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI);

    static void setInstrumentSlots(Instrument *instrument); // for Footswitchlooper

    static void tsunami_beat_playback(Instrument *instrument);

    static std::vector<int> total_vol; // for TopographyLog

    static void topography_midi_effects(Instrument *instrument, midi::MidiInterface<HardwareSerial>); // MIDI playback according to beat_topography

    // final tidy up functions: ---------------------------------------

    static void turnMidiNoteOff(Instrument* instrument, midi::MidiInterface<HardwareSerial>);
};
