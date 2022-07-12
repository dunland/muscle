#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// A.72 /////////////////////////////
void Score::run_PogoNumberOne(midi::MidiInterface<HardwareSerial> MIDI)
{

    switch (step)
    {
    case 0:
        if (setup)
        {
            Synthesizers::whammy->sendProgramChange(42, MIDI); // WHAMMY UP 2 OCT
            Hardware::footswitch_mode = Increment_Score;
            setup = false;
        }
        break;

    default:
        proceed_to_next_score();
        break;
    }
}