#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_alhambra(midi::MidiInterface<HardwareSerial> MIDI)
{
    static int program;
    static int valueXgoal, valueYgoal, valueX, valueY;
    switch (step)
    {
    case 0:
        if (setup_song())
        {
            Synthesizers::whammy->sendProgramChange(0, MIDI); // HARMONY UP 2 OCT
            Synthesizers::mKorg->sendProgramChange(34, MIDI); // load b_27
            Synthesizers::kaossPad3->sendControlChange(92, 127, MIDI); // Touch Pad on
            
        }
        // static int val = 0;
        // Synthesizers::kaossPad3->sendProgramChange(val, MIDI); // Touch Pad on
        // Hardware::lcd->setCursor(0,0);
        // Hardware::lcd->print(val);
        // delay(500);
        // val++;
        break;

    case 1:
        if (setup_song())
        {
            // program = int(random(97));
            program = 58;
            Synthesizers::kaossPad3->sendProgramChange(program, MIDI);
            
        }

        if (Drumset::snare->timing.wasHit){
            valueXgoal = int(random(128));
            valueYgoal = int(random(128));
        }

        // successive approximation:
        valueX = (valueX < valueXgoal) ? valueX + (valueXgoal - valueX) / 5 : valueX - (valueX - valueXgoal) / 5;
            Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_x, valueX, MIDI);
        valueY = (valueY < valueYgoal) ? valueY + (valueYgoal - valueY) / 5 : valueY - (valueY - valueYgoal) / 5;
            Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_y, valueY, MIDI);



        // Hardware::lcd->setCursor(9, 1);
        // Hardware::lcd->print("P:");
        // Hardware::lcd->setCursor(11, 1);
        // Hardware::lcd->print(program);
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
        step = 1;
        setup_state = true;
        break;
    }
}