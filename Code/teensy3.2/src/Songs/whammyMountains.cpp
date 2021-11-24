#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Score::run_whammyMountains(midi::MidiInterface<HardwareSerial> MIDI)
{
    /*
    WHAMMY MIDI CHANNEL = 4
    "CLASSIC MODE" (switch up)
    */

    switch (step)
    {
    case 0:
        if (setup)
        {
            /* ------- general initialization routine -------------- */
            Hardware::footswitch_mode = Increment_Score;
            resetInstruments();
            notes.clear();

            Synthesizers::whammy->sendProgramChange(1, MIDI); // selects Whammy ↑2 OCT

            setup = false;
        }

        /* ---------------- general loop here ---------------------- */

        // just count upwards, slowly:

        static unsigned long lastUpCount = millis();
        if (millis() > lastUpCount + 500)
        {
            static int val = 0;
            val = (val + 1) % 127;
            Synthesizers::whammy->sendNoteOn(val, MIDI);
            lastUpCount = millis();

            // print val
            Hardware::lcd->setCursor(0, 0);
            Hardware::lcd->print(val);
        }

        break;

    default:
        proceed_to_next_score();
        break;
    }
}