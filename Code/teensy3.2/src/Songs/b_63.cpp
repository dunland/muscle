#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

void run_b_63(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (Globals::active_song->step)
    {
    case 0:
        Synthesizers::mKorg->sendControlChange(mKORG_Arpeggio_onOff, 127, MIDI); // Apreggiator on
        Drumset::snare->set_effect(TapTempo);
        break;

    default:
        break;
    }
}