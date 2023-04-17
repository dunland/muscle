#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_alhambra(midi::MidiInterface<HardwareSerial> MIDI)
{
    static int valueXgoal, valueYgoal, valueX, valueY;

    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::whammy->sendProgramChange(0, MIDI); // HARMONY UP 2 OCT
            Synthesizers::mKorg->sendProgramChange(34, MIDI); // load b_27
        }
        break;

    case 1:
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::kaossPad3->sendControlChange(92, 127, MIDI); // Touch Pad on
            Synthesizers::kaossPad3->sendProgramChange(58, MIDI);
        }

        if (Drumset::snare->timing.wasHit)
        {
            valueXgoal = int(random(128));
            valueYgoal = int(random(128));
        }

        // successive approximation:
        valueX = (valueX < valueXgoal) ? valueX + (valueXgoal - valueX) / 5 : valueX - (valueX - valueXgoal) / 5;
        Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_x, valueX, MIDI);
        valueY = (valueY < valueYgoal) ? valueY + (valueYgoal - valueY) / 5 : valueY - (valueY - valueYgoal) / 5;
            Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_y, valueY, MIDI);

        Hardware::lcd->setCursor(6, 0);
        Hardware::lcd->print("X:");
        Hardware::lcd->setCursor(8, 0);
        Hardware::lcd->print(valueX);
        Hardware::lcd->setCursor(12, 0);
        Hardware::lcd->print("Y:");
        Hardware::lcd->setCursor(14, 0);
        Hardware::lcd->print(valueY);
        break;

    default:
        Synthesizers::kaossPad3->sendControlChange(92, 0, MIDI); // Touch Pad off
        Globals::active_song->proceed_to_next_score();
        break;
    }
}