#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_kupferUndGold(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch(Song::step)
    {
    case 0: // Intro
        if (Song::setup)
        {
            Synthesizers::whammy->sendProgramChange(62, MIDI); // HARMONY UP OCT → UP 2 OCT
            Song::setup = false;
        }
        break;

    case 1: // Fetz
        if (Song::setup)
        {
            Synthesizers::whammy->sendProgramChange(58, MIDI); // HARMONY UP 5TH → UP  OCT
            Song::setup = false;
        }
        break;

    case 2: // Gesangpart
        if (Song::setup)
        {
            Synthesizers::whammy->sendProgramChange(62, MIDI); // HARMONY UP OCT → UP 2 OCT
            Song::setup = false;
        }
        break;

    case 3: // Groove
        if (Song::setup)
        {
            Synthesizers::whammy->sendProgramChange(61, MIDI); // HARMONY UP OCT → UP 10TH
            Song::setup = false;
        }
        break;

    case 4: // Finale
        if (Song::setup)
        {
            Synthesizers::whammy->sendProgramChange(62, MIDI); // HARMONY UP OCT → UP 2 OCT
            Song::setup = false;
        }
        break;

    default:
        Song::proceed_to_next_score();
        break;
    }
}