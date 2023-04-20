#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
/*
    DD-200 test code compendium
*/

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_intro(midi::MidiInterface<HardwareSerial> MIDI)
{

    static int valueXgoal = 64, valueYgoal = 64, valueX, valueY;

    switch (step)
    {

    case 0: // some ramp effect on snare. Used as intro for Concert at Stasi's. Works fine with DD200-DUAL mode @ ~150 BPM quarter notes

        if (setup)
        {
            Hardware::footswitch_mode = Increment_Score;
            resetInstruments();
            notes.clear();

            Drumset::hihat->set_effect(TapTempo);
            Drumset::snare->setup_midi(dd200_DelayTime, Synthesizers::dd200, 89, 0, -9.96, 0.08);
            Drumset::snare->set_effect(Change_CC);

            Synthesizers::mKorg->sendProgramChange(78, MIDI); // b.27
            Synthesizers::kaossPad3->sendProgramChange(58, MIDI);

            Synthesizers::kaossPad3->sendControlChange(92, 127, MIDI); // Touch Pad on


            setup = false;
        }

        if (Drumset::snare->timing.wasHit)
        {
            valueX = 127;
            valueY = 127;
        }

        if (Drumset::kick->timing.wasHit)
        {
            valueX = 0;
            valueY = 127;
        }

        // successive approximation:
        valueX = (valueX < valueXgoal) ? valueX + (valueXgoal - valueX) / 5 : valueX - (valueX - valueXgoal) / 5;
        Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_x, valueX, MIDI);
        valueY = (valueY < valueYgoal) ? valueY + (valueYgoal - valueY) / 5 : valueY - (valueY - valueYgoal) / 5;
        Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_y, valueY, MIDI);

        /* CC-values are printed automatically */

        break;

    default:
        Synthesizers::kaossPad3->sendControlChange(92, 0, MIDI); // Touch Pad off
        proceed_to_next_score();
        step = 0;
        break;
    }
}