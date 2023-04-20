#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_theodolit(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        if (setup)
        {
            Synthesizers::dd200->sendProgramChange(3, MIDI);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0, MIDI); // ATTENTION: must be after programChange!
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 14, MIDI); // 518 BPM

            Synthesizers::whammy->sendProgramChange(58, MIDI); // HARMONY 5TH → UP OCT
            setup = false;
        }
        break;

    default:
        proceed_to_next_score();
        break;
    }
}