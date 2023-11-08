#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// ZITTERAAL /////////////////////////////
void run_besen()
{
    switch(Globals::active_song->step)
    {
    case 0: // Snare → Vocoder (D+F)
        if (Globals::active_song->get_setup_state())
        {
            Hardware::footswitch_mode = Increment_Score;
            Globals::active_song->resetInstruments();
            Globals::active_song->notes.clear();

            Drumset::hihat->set_effect(TapTempo);
            Synthesizers::dd200->sendProgramChange(3);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0);

            Synthesizers::mKorg->sendProgramChange(62); // corresponds A.87
            Synthesizers::mKorg->sendControlChange(mKORG_Amplevel, 80); // set loudness to 80
            Synthesizers::whammy->sendProgramChange(83); // whammy aus

            delay(50);
            Synthesizers::mKorg->sendNoteOn(Note_D6);
            Synthesizers::mKorg->sendNoteOn(Note_F6);

        }

        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(Globals::current_BPM);
        Hardware::lcd->setCursor(10, 1);
        Hardware::lcd->print("Voc");

        break;

    case 1: // Teil B: MIDI-Clock aus
        if (Globals::active_song->get_setup_state())
        {
            Drumset::hihat->set_effect(Monitor); // no more tempo trigger
        }

        Hardware::lcd->setCursor(10, 1);
        Hardware::lcd->print("stop");

        break;

    // case 2: // finale: crash 2 triggert noten

    //     if (Globals::active_song->setup)
    //     {
    //         Globals::active_song->setup = false;
    //         Synthesizers::mKorg->sendProgramChange(42; // corresponds A.63
    //         delay(50);
    //         Synthesizers::mKorg->sendNoteOn(Note_D6;
    //         Synthesizers::mKorg->sendNoteOn(Note_G6;
    //     }

    //     static int sub_part = 0;
    //     static unsigned long lastNoteChange = 0;

    //     if (Drumset::crash1->timing.wasHit && millis() > lastNoteChange + 3000)
    //     {
    //         if (sub_part == 0)
    //         {
    //             Synthesizers::mKorg->sendNoteOff(Note_D6;
    //             Synthesizers::mKorg->sendNoteOff(Note_G6;

    //             Synthesizers::mKorg->sendNoteOn(Note_D6;
    //             Synthesizers::mKorg->sendNoteOn(Note_Bb5;

    //             sub_part = 1;
    //         }

    //         else if (sub_part == 1)
    //         {
    //             Synthesizers::mKorg->sendNoteOff(Note_D6;
    //             Synthesizers::mKorg->sendNoteOff(Note_Bb5;

    //             Synthesizers::mKorg->sendNoteOn(Note_D6;
    //             Synthesizers::mKorg->sendNoteOn(Note_C7;

    //             sub_part = 2;
    //         }

    //         else if (sub_part == 2)
    //         {
    //             Synthesizers::mKorg->sendNoteOff(Note_D6;
    //             Synthesizers::mKorg->sendNoteOff(Note_C7;

    //             Synthesizers::mKorg->sendNoteOn(Note_D6;
    //             Synthesizers::mKorg->sendNoteOn(Note_G6;

    //             sub_part = 0;
    //         }

    //         lastNoteChange = millis();
    //     }

    //     Hardware::lcd->setCursor(0, 0);
    //     Hardware::lcd->print(Globals::current_BPM);
    //     Hardware::lcd->setCursor(10, 1);
    //     Hardware::lcd->print("arp");

    //     break;

    // case 3:
    //     if (Globals::active_song->setup)
    //     {
    //         Synthesizers::mKorg->sendNoteOff(Note_D6;
    //         Synthesizers::mKorg->sendNoteOff(Note_G6;
    //         Synthesizers::mKorg->sendNoteOff(Note_Bb5;
    //         Synthesizers::mKorg->sendNoteOff(Note_C7;
    //         Synthesizers::mKorg->sendProgramChange(42; // restart A.63 to stop notes
    //     }

    //     Hardware::lcd->setCursor(10, 1);
    //     Hardware::lcd->print("end");
    //     break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}