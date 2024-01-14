#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// ZITTERAAL /////////////////////////////
void run_sturmUndDrang()
{

    static float mKorgAmpLvl;

    switch (Globals::active_song->step)
    {
    case 0: // nichts
        if (Globals::active_song->get_setup_state())
        {
            Drumset::hihat->set_effect(TapTempo);
        }

        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(Globals::current_BPM);

        break;

    case 1: // Vocoder, nur D spielend
        if (Globals::active_song->get_setup_state())
        {
            Globals::active_song->resetInstruments();
            Globals::active_song->notes.clear();

            // Synthesizers::dd200->sendProgramChange(3);
            // Synthesizers::dd200->sendControlChange(dd200_OnOff, 0);

            Synthesizers::mKorg->sendProgramChange(57); // corresponds A.82 // TODO: create enum with all programs...
            // Synthesizers::whammy->sendProgramChange(83); // whammy aus
            Synthesizers::mKorg->sendControlChange(mKORG_Amplevel, 80);

            delay(50);
            Synthesizers::mKorg->sendNoteOn(Note_D5);
            mKorgAmpLvl = 0;

            // Synthesizers::dd200->sendControlChange(dd200_DelayTime, ) // Timing für DD200 einstellen? 145BPM bzw 415ms.. kann man leider nicht mit MIDI so genau einstellen.
        }

        // TODO: anschwellende synt-latustaerke
        if (mKorgAmpLvl < 102)
        {
            mKorgAmpLvl += 0.5;
            Synthesizers::mKorg->sendControlChange(mKORG_Amplevel, int(mKorgAmpLvl));
        }

        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(Globals::current_BPM);
        Hardware::lcd->setCursor(3, 0);
        Hardware::lcd->print("D");
        Hardware::lcd->setCursor(5, 0);
        Hardware::lcd->print("V");
        Hardware::lcd->setCursor(6, 0);
        Hardware::lcd->print(int(mKorgAmpLvl));

        break;

    case 2: // Vocoder D,D,F
        if (Globals::active_song->get_setup_state())
        {
            Globals::active_song->resetInstruments();
            Globals::active_song->notes.clear();

            // Synthesizers::dd200->sendProgramChange(3);
            // Synthesizers::dd200->sendControlChange(dd200_OnOff, 0);

            Synthesizers::mKorg->sendProgramChange(57); // corresponds A.82 // TODO: create enum with all programs...

            delay(50);
            Synthesizers::mKorg->sendNoteOff(Note_D5);
            Synthesizers::mKorg->sendNoteOn(Note_D3);
            Synthesizers::mKorg->sendNoteOn(Note_D4);
            Synthesizers::mKorg->sendNoteOn(Note_F4);
        }

        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(Globals::current_BPM);
        Hardware::lcd->setCursor(3, 0);
        Hardware::lcd->print("D+D+F");

        break;

    case 3: // alles zerhechseln
        if (Globals::active_song->get_setup_state())
        {
            Globals::active_song->notes.clear();
            Synthesizers::mKorg->sendProgramChange(0); // workaround: disable arp...

            Drumset::standtom->set_effect(Change_CC);
            Drumset::standtom->setup_midi(dd200_mod, Synthesizers::dd200, 127, 0, 4, -0.02);

            Drumset::kick->set_effect(Change_CC);
            Drumset::kick->setup_midi(dd200_DelayTime, Synthesizers::dd200, 107, 20, 10, -0.2);

            Drumset::snare->setup_midi(whammyPedal, Synthesizers::whammy, 127, 0, 20, -1);
            Drumset::snare->set_effect(Change_CC);
        }

        Hardware::lcd->setCursor(10, 1);
        Hardware::lcd->print("stop");

        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}