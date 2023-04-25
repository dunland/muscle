#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// A.72 /////////////////////////////
void Song::run_wueste(midi::MidiInterface<HardwareSerial> MIDI)
{

    switch (step)
    {
    case 0:
        if (setup)
        {
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 13, MIDI);
            Synthesizers::dd200->sendProgramChange(1, MIDI);  // Program #1
            Hardware::footswitch_mode = Increment_Score;
            setup = false;
        }
        break;

    default:
        proceed_to_next_score();
        break;
    }
}