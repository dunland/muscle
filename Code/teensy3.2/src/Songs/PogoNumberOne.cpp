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
            Synthesizers::whammy->sendProgramChange(43, MIDI); // WHAMMY UP 2 OCT
            Hardware::footswitch_mode = Increment_Score;
            setup = false;
        }
        break;

    case 1:
        break;

    default:
        Globals::machine_state = Running;
        proceed_to_next_score();
        break;
    }
}