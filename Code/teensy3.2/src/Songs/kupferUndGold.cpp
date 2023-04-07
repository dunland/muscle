#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_kupferUndGold(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0: // Intro
        if (setup_song())
        {
            Synthesizers::whammy->sendProgramChange(62, MIDI); // HARMONY UP OCT → UP 2 OCT
            
        }
        break;

    case 1: // Fetz
        if (setup_song())
        {
            Synthesizers::whammy->sendProgramChange(58, MIDI); // HARMONY UP 5TH → UP  OCT
            
        }
        break;

    case 2: // Gesangpart
        if (setup_song())
        {
            Synthesizers::whammy->sendProgramChange(62, MIDI); // HARMONY UP OCT → UP 2 OCT
            
        }
        break;

    case 3: // Groove
        if (setup_song())
        {
            Synthesizers::whammy->sendProgramChange(61, MIDI); // HARMONY UP OCT → UP 10TH
            
        }
        break;

    case 4: // Finale
        if (setup_song())
        {
            Synthesizers::whammy->sendProgramChange(62, MIDI); // HARMONY UP OCT → UP 2 OCT
            
        }
        break;

    default:
        proceed_to_next_score();
        break;
    }
}