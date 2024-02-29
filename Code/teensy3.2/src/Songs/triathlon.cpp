#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_triathlon()
{

    switch (Globals::active_song->step)
    {
    case 0: // A
        if (Globals::active_song->get_setup_state())
        {
            Globals::current_BPM = 137;
            Globals::tapInterval = 60000 / Globals::current_BPM;
            Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128);

            Synthesizers::dd200->sendProgramChange(6);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127); // ON
        }
        break;

    case 1: // B1
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0); // OFF
            Synthesizers::whammy->sendControlChange(whammy_CHORDS_OCT_2OCT_OFF, 0); // OFF
        }
        break;

    case 2: // A
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127); // ON
            Synthesizers::whammy->sendControlChange(whammy_CHORDS_OCT_2OCT_ON, 127); // ON
        }
        break;

    case 3: // B2
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0); // OFF
            Synthesizers::whammy->sendControlChange(whammy_CHORDS_OCT_2OCT_OFF, 0); // OFF
        }
        break;

    case 4: // LOOPISTAMPF
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127); // ON
        }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}