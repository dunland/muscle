#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_roeskur(midi::MidiInterface<HardwareSerial> MIDI)
{

    // static Synthesizer *mKorg = Synthesizers::mKorg;
    // static int noteIdx = 0;

    switch(Globals::active_song->step)
    {
    case 0:
    if (Globals::active_song->get_setup_state())
    {
        Synthesizers::whammy->sendProgramChange(62, MIDI); // HARMONY UP OCT / UP 2 OCT
        
    }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}