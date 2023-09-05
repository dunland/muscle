#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_ferdinandPiech()
{
    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::dd200->sendProgramChange(6);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127);
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 19); // 84 BPM
        }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}