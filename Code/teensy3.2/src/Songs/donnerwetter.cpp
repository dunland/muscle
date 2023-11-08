#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>
/*
    DD-200 test code compendium
*/

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_donnerwetter()
{
    switch (Globals::active_song->step)
    {

    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0);
            Synthesizers::whammy->sendProgramChange(83); // Whammy off!!
        }
        break;

    case 1: // some ramp effect on snare. Works fine with DD200-DUAL mode @ ~150 BPM quarter notes

        if (Globals::active_song->get_setup_state())
        {
            Hardware::footswitch_mode = Increment_Score;
            Globals::active_song->resetInstruments();
            Globals::active_song->notes.clear();

            Drumset::hihat->set_effect(TapTempo);
            Drumset::snare->setup_midi(dd200_DelayTime, Synthesizers::dd200, 89, 0, -9.96, 0.08);
            Drumset::snare->set_effect(Change_CC);
        }

        /* CC-values are printed automatically */

        Hardware::lcd->setCursor(4, 1);
        Hardware::lcd->print("DUAL snaRamp");

        break;

    default:
        Synthesizers::mKorg->sendNoteOff(31);
        Globals::active_song->proceed_to_next_score();
        Globals::active_song->step = 0;
        break;
    }
}