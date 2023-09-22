#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// A.72 /////////////////////////////
void run_wueste()
{

    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 13);
            Synthesizers::dd200->sendProgramChange(1);  // Program #1
            Hardware::footswitch_mode = Increment_Score;
        }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}