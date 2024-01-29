#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// ZITTERAAL /////////////////////////////
void run_sturmUndDrang()
{
    switch (Globals::active_song->step)
    {
    case 0: // nichts
        if (Globals::active_song->get_setup_state())
        {
            Drumset::hihat->set_effect(TapTempo);
            for (auto &instrument : Drumset::instruments)
            {
                for (auto &midiTarget : instrument->midiTargets)
                {
                    delete midiTarget;
                    midiTarget = nullptr;
                }
                instrument->midiTargets.clear();
            }
            Devtools::println_to_console("Sturm&Drang: All midiTargets deleted!");
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
        }

        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(Globals::current_BPM);
        Hardware::lcd->setCursor(3, 0);
        Hardware::lcd->print("D");

        break;

    case 2: // Vocoder D,D,F
        if (Globals::active_song->get_setup_state())
        {
            Globals::active_song->resetInstruments();
            Globals::active_song->notes.clear();

            // Synthesizers::dd200->sendProgramChange(3);
            // Synthesizers::dd200->sendControlChange(dd200_OnOff, 0);

            Synthesizers::mKorg->sendProgramChange(57); // corresponds A.82 // TODO: create enum with all programs...
            Synthesizers::mKorg->sendControlChange(mKORG_Amplevel, 102);

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
            Drumset::standtom->addMidiTarget(dd200_mod, Synthesizers::dd200, 127, 0, 4, -0.02);

            Drumset::kick->set_effect(Change_CC);
            Drumset::kick->addMidiTarget(dd200_DelayTime, Synthesizers::dd200, 107, 20, 10, -0.2);
            Drumset::kick->addMidiTarget(mKORG_Mod_FX_Depth, Synthesizers::mKorg, 82, 127, 4, 1);

            Drumset::snare->addMidiTarget(whammyPedal, Synthesizers::whammy, 127, 0, 20, -1);
            Drumset::snare->addMidiTarget(mKORG_LFO_Speed, Synthesizers::mKorg, 10, 100, 20, 1);
            Drumset::snare->set_effect(Change_CC);

            for (auto &instrument : {Drumset::snare, Drumset::standtom, Drumset::kick})
            {
                Devtools::print_to_console("midiTargets of ");
                Devtools::print_to_console(Globals::DrumtypeToHumanreadable(instrument->drumtype));
                Devtools::print_to_console(": ");
                for (auto &midiTarget : instrument->midiTargets)
                {
                    Devtools::print_to_console(midiTarget->synth->name);
                    Devtools::print_to_console(" ");
                }
                Devtools::println_to_console("");
            }
        }

        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}