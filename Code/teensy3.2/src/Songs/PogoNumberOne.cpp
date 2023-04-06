#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// A.72 /////////////////////////////
void run_PogoNumberOne(midi::MidiInterface<HardwareSerial> MIDI)
{

    switch(Song::step)
    {
    case 0:
        if (Song::setup)
        {
            Synthesizers::whammy->sendProgramChange(42, MIDI); // WHAMMY UP 2 OCT
            Hardware::footswitch_mode = Increment_Score;
            Song::setup = false;
        }
        break;

    default:
        Song::proceed_to_next_score();
        break;
    }
}