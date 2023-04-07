#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// A.72 /////////////////////////////
void Song::run_PogoNumberOne(midi::MidiInterface<HardwareSerial> MIDI)
{

    switch (step)
    {
    case 0:
        if (setup_song())
        {
            Synthesizers::whammy->sendProgramChange(42, MIDI); // WHAMMY UP 2 OCT
            Hardware::footswitch_mode = Increment_Score;
            
        }
        break;

    default:
        proceed_to_next_score();
        break;
    }
}