#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// A.72 /////////////////////////////
void run_PogoNumberOne()
{

    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::mKorg->sendProgramChange(3); // just to kill the Arpeggiator...

            Globals::bSendMidiClock = false;

            Synthesizers::whammy->sendProgramChange(42); // WHAMMY UP 2 OCT

            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 4);
            Synthesizers::dd200->sendProgramChange(3); // Program #3

            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0); // ATTENTION: must be after programChange!
            Hardware::footswitch_mode = Increment_Score;
        }
        break;

    default:
        Globals::bSendMidiClock = true;
        Globals::active_song->proceed_to_next_score();
        break;
    }
}