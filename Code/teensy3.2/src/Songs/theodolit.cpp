#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_theodolit(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch(Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->setup)
        {
            Synthesizers::whammy->sendProgramChange(58, MIDI); // HARMONY 5TH → UP OCT
            Globals::active_song->setup = false;
        }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}