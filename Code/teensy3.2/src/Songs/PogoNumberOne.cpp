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
        if (setup)
        {
            Globals::bSendMidiClock = false;

            Synthesizers::whammy->sendProgramChange(42, MIDI); // WHAMMY UP 2 OCT

            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 4, MIDI);
            Synthesizers::dd200->sendProgramChange(3, MIDI);  // Program #3

            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0, MIDI); // ATTENTION: must be after programChange!
            Hardware::footswitch_mode = Increment_Score;
            setup = false;
        }
        break;

    default:
        Globals::bSendMidiClock = true;
        proceed_to_next_score();
        break;
    }
}