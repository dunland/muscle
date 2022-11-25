#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
/*
    DD-200 test code compendium
*/

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_donnerwetter(midi::MidiInterface<HardwareSerial> MIDI)
{
    static float delay_time = 0;
    static float delay_depth = 0;
    static float delay_level = 0;

    switch (step)
    {

    case 0: // nothing

        break;

    case 1: // some ramp effect on snare. Works fine with DD200-DUAL mode @ ~150 BPM quarter notes

        if (setup)
        {
            Hardware::footswitch_mode = Increment_Score;
            resetInstruments();
            notes.clear();

            Drumset::hihat->set_effect(Monitor);
            Drumset::snare->setup_midi(dd200_DelayTime, Synthesizers::dd200, 89, 0, -9.96, 0.08);
            Drumset::snare->set_effect(Change_CC);

            setup = false;
        }

        /* CC-values are printed automatically */

        Hardware::lcd->setCursor(4, 1);
        Hardware::lcd->print("DUAL snaRamp");

        break;

    default:
        Synthesizers::mKorg->sendNoteOff(31, MIDI);
        proceed_to_next_score();
        step = 0;
        break;
    }
}