#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// ZITTERAAL /////////////////////////////
void run_zitteraal(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch(Globals::active_song->step)
    {
    case 0: // Snare → Vocoder (D+F)
        if (Globals::active_song->setup)
        {
            Hardware::footswitch_mode = Increment_Score;
            Globals::active_song->resetInstruments();
            Globals::active_song->notes.clear();

            Drumset::hihat->set_effect(TapTempo);

            Synthesizers::mKorg->sendProgramChange(62, MIDI); // corresponds A.87
            Synthesizers::mKorg->sendControlChange(mKORG_Amplevel, 80, MIDI); // set loudness to 80
            Synthesizers::whammy->sendProgramChange(58, MIDI); // HARMONY UP 5TH / UP 6TH

            delay(50);
            Synthesizers::mKorg->sendNoteOn(Note_D6, MIDI);
            Synthesizers::mKorg->sendNoteOn(Note_F6, MIDI);
            Globals::active_song->setup = false;
        }

        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(Globals::current_BPM);
        Hardware::lcd->setCursor(10, 1);
        Hardware::lcd->print("Voc");

        break;

    case 1: // turn notes off
        if (Globals::active_song->setup)
        {
            Synthesizers::mKorg->sendNoteOff(Note_D6, MIDI);
            Synthesizers::mKorg->sendNoteOff(Note_F6, MIDI);
            Synthesizers::mKorg->sendProgramChange(62, MIDI); // restart to stop Arp
            Globals::active_song->setup = false;
        }

        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(Globals::current_BPM);
        Hardware::lcd->setCursor(10, 1);
        Hardware::lcd->print("stop");

        break;

    // case 2: // finale: crash 2 triggert noten

    //     if (Globals::active_song->setup)
    //     {
    //         Globals::active_song->setup = false;
    //         Synthesizers::mKorg->sendProgramChange(42, MIDI); // corresponds A.63
    //         delay(50);
    //         Synthesizers::mKorg->sendNoteOn(Note_D6, MIDI);
    //         Synthesizers::mKorg->sendNoteOn(Note_G6, MIDI);
    //     }

    //     static int sub_part = 0;
    //     static unsigned long lastNoteChange = 0;

    //     if (Drumset::crash1->timing.wasHit && millis() > lastNoteChange + 3000)
    //     {
    //         if (sub_part == 0)
    //         {
    //             Synthesizers::mKorg->sendNoteOff(Note_D6, MIDI);
    //             Synthesizers::mKorg->sendNoteOff(Note_G6, MIDI);

    //             Synthesizers::mKorg->sendNoteOn(Note_D6, MIDI);
    //             Synthesizers::mKorg->sendNoteOn(Note_Bb5, MIDI);

    //             sub_part = 1;
    //         }

    //         else if (sub_part == 1)
    //         {
    //             Synthesizers::mKorg->sendNoteOff(Note_D6, MIDI);
    //             Synthesizers::mKorg->sendNoteOff(Note_Bb5, MIDI);

    //             Synthesizers::mKorg->sendNoteOn(Note_D6, MIDI);
    //             Synthesizers::mKorg->sendNoteOn(Note_C7, MIDI);

    //             sub_part = 2;
    //         }

    //         else if (sub_part == 2)
    //         {
    //             Synthesizers::mKorg->sendNoteOff(Note_D6, MIDI);
    //             Synthesizers::mKorg->sendNoteOff(Note_C7, MIDI);

    //             Synthesizers::mKorg->sendNoteOn(Note_D6, MIDI);
    //             Synthesizers::mKorg->sendNoteOn(Note_G6, MIDI);

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
    //         Synthesizers::mKorg->sendNoteOff(Note_D6, MIDI);
    //         Synthesizers::mKorg->sendNoteOff(Note_G6, MIDI);
    //         Synthesizers::mKorg->sendNoteOff(Note_Bb5, MIDI);
    //         Synthesizers::mKorg->sendNoteOff(Note_C7, MIDI);
    //         Synthesizers::mKorg->sendProgramChange(42, MIDI); // restart A.63 to stop notes
    //     }

    //     Hardware::lcd->setCursor(10, 1);
    //     Hardware::lcd->print("end");
    //     break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}