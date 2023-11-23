#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_kupferUndGold()
{
    switch(Globals::active_song->step)
    {
    case 0: // Intro
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::dd200->sendProgramChange(4);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127);
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 3);
            Synthesizers::whammy->sendProgramChange(62); // HARMONY UP OCT → UP 2 OCT
            Globals::current_BPM = 81;
            Globals::tapInterval = 740;
            Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM

        }
        break;

    case 1: // Fetz
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::whammy->sendProgramChange(58); // HARMONY UP 5TH → UP  OCT
        }
        break;

    case 2: // Gesangpart
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::whammy->sendProgramChange(62); // HARMONY UP OCT → UP 2 OCT

        }
        break;

    case 3: // Groove
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::whammy->sendProgramChange(61); // HARMONY UP OCT → UP 10TH

        }
        break;

    case 4: // Finale
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::whammy->sendProgramChange(62); // HARMONY UP OCT → UP 2 OCT

        }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}