#include <Score.h>
#include <MIDI.h>

// TODO:
//TOPOGRAPHY Score::beat_regularity;

TOPOGRAPHY Score::beat_sum;

void Score::add_bassNote(int note, int note_iterator_)
{
    notes.push_back(note);
    note_iterator = note_iterator_;
}

///////////////////////////////////////////////////////////////////////
////////////////////////////// MODES //////////////////////////////////
///////////////////////////////////////////////////////////////////////

void Score::continuousBassNote(midi::MidiInterface<HardwareSerial> MIDI, int note_length) // initiates a continuous bass note from score
{
    if (Globals::current_beat_pos % note_length == 0)
    {
        MIDI.sendNoteOn(notes[0], 127, Volca);
    }
    else
        MIDI.sendNoteOff(notes[0], 127, Volca);

    if (Globals::current_beat_pos == note_iterator) // change bass note each bar
        if (notes.size() > 1)
            note_idx = (note_idx + 1) % notes.size(); // iterate through the bass notes
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
    delaytime = min(delaytime, 127);                                               // must not be greater than 127
    MIDI.sendControlChange(51, delaytime, 2);
}
