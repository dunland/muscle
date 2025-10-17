#include "Globals.h"
#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_norbert()
{
    // TODO: ACHTUNG!! DARF NICHT ERSTER SONG SEIN: CRASHT SONST!
    switch (Globals::active_song->step)
    {
    case 0: // A
        if (Globals::active_song->isStateInit())
        {
            Globals::current_BPM = 143; // on recording: 137;
            Globals::tapInterval = 60000 / Globals::current_BPM;
            Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128);

            Synthesizers::kaossPad3->sendProgramChange(70);
            Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_x, 105);
            Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_y, 60);
            Synthesizers::kaossPad3->sendControlChange(KP3_FX_Depth, 35);

            Synthesizers::kaossPad3->sendControlChange(KP3_Hold, 1);
            Synthesizers::dd200->sendProgramChange(6);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127);            // ON
            Synthesizers::whammy->sendProgramChange(whammy_CHORDS_OCT_2OCT_OFF); // 83

            Drumset::snare->addMidiTarget(visuals_snare, Synthesizers::visuals,
                                    127, 0, 10, -0.1);
            Drumset::snare->set_effect(Change_CC);
        }

        Hardware::lcd->setCursor(9, 1);
        Hardware::lcd->print(".A1");

        break;

    case 1: // B1
        if (Globals::active_song->isStateInit())
        {
            Synthesizers::kaossPad3->sendControlChange(KP3_Hold, 0);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0); // OFF
            Synthesizers::whammy->sendProgramChange(whammy_CHORDS_OCT_2OCT_OFF);
        }

        Hardware::lcd->setCursor(9, 1);
        Hardware::lcd->print(".B1");

        break;

    case 2: // A
        if (Globals::active_song->isStateInit())
        {
            Synthesizers::kaossPad3->sendControlChange(KP3_Hold, 1);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127);           // ON
            Synthesizers::whammy->sendProgramChange(whammy_CHORDS_OCT_2OCT_ON); // ON
            delay(200);
        }

        Hardware::lcd->setCursor(9, 1);
        Hardware::lcd->print(".A2");

        break;

    case 3: // B2
        if (Globals::active_song->isStateInit())
        {
            Synthesizers::kaossPad3->sendControlChange(KP3_Hold, 0);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0);              // OFF
            Synthesizers::whammy->sendProgramChange(whammy_CHORDS_OCT_2OCT_OFF); // 83
        }

        Hardware::lcd->setCursor(9, 1);
        Hardware::lcd->print(".B2");

        break;

    case 4: // LOOPISTAMPF
        if (Globals::active_song->isStateInit())
        {
            Synthesizers::whammy->sendProgramChange(whammy_CHORDS_OCT_2OCT_ON);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127); // ON
            Synthesizers::mKorg->sendProgramChange(85);               // b.36
        }

        Hardware::lcd->setCursor(9, 1);
        Hardware::lcd->print(".loop");

        break;

    case 5: // A3: KP3 wieder an

        if (Globals::active_song->isStateInit())
        {
            Synthesizers::kaossPad3->sendControlChange(KP3_Hold, 1);
        }

        Hardware::lcd->setCursor(9, 1);
        Hardware::lcd->print(".A3");

        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}