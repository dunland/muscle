#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_ferdinandPiech(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        if (setup)
        {
            Synthesizers::dd200->sendProgramChange(6, MIDI);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127, MIDI);
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 19, MIDI); // 84 BPM

            setup = false;
        }
        break;

    default:
        proceed_to_next_score();
        break;
    }
}