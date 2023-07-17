#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_kupferUndGold(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch(Globals::active_song->step)
    {
    case 0: // Intro
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::dd200->sendProgramChange(4, MIDI);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127, MIDI);
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 3, MIDI);
            Synthesizers::whammy->sendProgramChange(62, MIDI); // HARMONY UP OCT → UP 2 OCT
            
        }
        break;

    case 1: // Fetz
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::whammy->sendProgramChange(58, MIDI); // HARMONY UP 5TH → UP  OCT
            
        }
        break;

    case 2: // Gesangpart
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::whammy->sendProgramChange(62, MIDI); // HARMONY UP OCT → UP 2 OCT
            
        }
        break;

    case 3: // Groove
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::whammy->sendProgramChange(61, MIDI); // HARMONY UP OCT → UP 10TH
            
        }
        break;

    case 4: // Finale
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::whammy->sendProgramChange(62, MIDI); // HARMONY UP OCT → UP 2 OCT
            
        }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}