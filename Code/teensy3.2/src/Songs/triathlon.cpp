#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

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

            Synthesizers::kaossPad3->sendProgramChange(70);
            Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_x, 105);
            Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_y, 60);
            Synthesizers::kaossPad3->sendControlChange(KP3_FX_Depth, 35);
            
            Synthesizers::kaossPad3->sendControlChange(KP3_Hold, 1);
            Synthesizers::dd200->sendProgramChange(6);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127); // ON
            Synthesizers::whammy->sendProgramChange(whammy_CHORDS_OCT_2OCT_OFF); // 83
        }
        break;

    case 1: // B1
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::kaossPad3->sendControlChange(KP3_Hold, 0);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0); // OFF
            Synthesizers::whammy->sendProgramChange(whammy_CHORDS_OCT_2OCT_OFF);
        }
        break;

    case 2: // A
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::kaossPad3->sendControlChange(KP3_Hold, 1);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127); // ON
            Synthesizers::whammy->sendProgramChange(whammy_CHORDS_OCT_2OCT_ON); // ON
            delay(200);
        }
        break;

    case 3: // B2
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::kaossPad3->sendControlChange(KP3_Hold, 0);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0); // OFF
            Synthesizers::whammy->sendProgramChange(whammy_CHORDS_OCT_2OCT_OFF); // 83

        }
        break;

    case 4: // LOOPISTAMPF
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::whammy->sendProgramChange(whammy_CHORDS_OCT_2OCT_ON);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127); // ON
            Synthesizers::mKorg->sendProgramChange(85); // b.36
            Synthesizers::mKorg->sendNoteOn(Note_D4);
            Synthesizers::mKorg->sendNoteOn(Note_D5);
        }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}