#include <Score.h>
#include <MIDI.h>

// TOPOGRAPHY Score::beat_regularity;

void Score::continuousBassNote(midi::MidiInterface<HardwareSerial> MIDI) // initiates a continuous bass note from score
{
    if (Globals::current_beat_pos % 32 == 0)
        MIDI.sendNoteOn(notes[0], 127, 2);
    // else
        // MIDI.sendNoteOff(notes[0], 127, 2);
}

void Score::envelope_cutoff(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI)
{
    int cutoff_val = topography->a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    cutoff_val = max(20, cutoff_val);                                    // must be at least 20
    cutoff_val = min(cutoff_val, 127);                                   // must not be greater than 127
    MIDI.sendControlChange(44, cutoff_val, 2);
}

void Score::envelope_volume(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI)
{
    int amp_val = topography->a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    // amp_val = max(0, amp_val);                                    // must be at least 0
    amp_val = min(amp_val, 127);            // must not be greater than 127
    MIDI.sendControlChange(50, amp_val, 2); // set loudness
}

void Score::crazyDelays(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
    int delaytime = instrument->topography.a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    delaytime = min(delaytime, 127);                                           // must not be greater than 127
    MIDI.sendControlChange(51, delaytime, 2);
}

void Score::regularHighNote(Instrument *instrument)
{
}
