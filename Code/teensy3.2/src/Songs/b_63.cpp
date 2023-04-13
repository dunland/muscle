#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

void run_b_63(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::mKorg->sendProgramChange(106, MIDI);
            Synthesizers::mKorg->sendControlChange(mKORG_Arpeggio_onOff, 127, MIDI); // Apreggiator on
            Drumset::snare->set_effect(TapTempo);

            Globals::active_song->playSingleNote(Synthesizers::mKorg, MIDI);

        }
        break;

    default: // just restart
        Globals::active_song->setup_state = true;
        Globals::active_song->step = 0;
        break;
    }
}