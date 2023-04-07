#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// A.72 /////////////////////////////
void run_PogoNumberOne(midi::MidiInterface<HardwareSerial> MIDI)
{

    switch(Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->setup)
        {
            Synthesizers::whammy->sendProgramChange(42, MIDI); // WHAMMY UP 2 OCT
            Hardware::footswitch_mode = Increment_Score;
            Globals::active_song->setup = false;
        }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}