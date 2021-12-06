#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Score::run_hutschnur(midi::MidiInterface<HardwareSerial> MIDI)
{

    static Synthesizer *mKorg = Synthesizers::mKorg;
    static int noteIdx = 0;

    switch (step)
    {
    case 0:
        break;

    case 1: // start Vocoder A.86
        if (setup)
        {
            /* ------- general initialization routine -------------- */
            Hardware::footswitch_mode = Increment_Score;
            resetInstruments();
            notes.clear();

            mKorg->sendProgramChange(61, MIDI); // selects mKORG Voice A.26

            notes.push_back(28); // E
            notes.push_back(40); // E

            notes.push_back(42); // Fis
            notes.push_back(45); // A

            notes.push_back(45); // A
            notes.push_back(43); // G

            notes.push_back(43); // G
            notes.push_back(38); // G

            setup = false;
        }

        /* ---------------- general loop here ---------------------- */

        // if (Drumset::snare->timing.wasHit)
        // {
        //     mKorg->sendNoteOn(notes[noteIdx], MIDI);
        //     mKorg->sendNoteOn(notes[noteIdx + 1], MIDI);
        // }

        // change notes via crash
        static unsigned long lastCrashHit = 0;
        if (Drumset::crash1->timing.wasHit)
        {
            if (millis() > lastCrashHit + 3000)
            {

                // turn last notes off:
                if (noteIdx != 0)
                {
                    mKorg->sendNoteOff(notes[noteIdx - 2], MIDI);
                    mKorg->sendNoteOff(notes[noteIdx - 1], MIDI);
                }
                else if (noteIdx == 0)
                {
                    mKorg->sendNoteOff(notes[7], MIDI);
                    mKorg->sendNoteOff(notes[8], MIDI);
                }

                // increase noteIdx
                noteIdx = (noteIdx + 2) % 8;
                lastCrashHit = millis();
            }
        }

        // send noteOn permanently
        mKorg->sendNoteOn(notes[noteIdx], MIDI);
        mKorg->sendNoteOn(notes[noteIdx + 1], MIDI);

        // print info to display:
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(noteIdx);

        Hardware::lcd->setCursor(2, 0);
        Hardware::lcd->print(notes[noteIdx]);

        Hardware::lcd->setCursor(5, 0);
        Hardware::lcd->print(notes[noteIdx + 1]);

        break;

    default:
        increase_step();
        break;
    }
}