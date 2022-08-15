#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_alhambra(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        if (setup)
        {
            Synthesizers::whammy->sendProgramChange(0, MIDI); // HARMONY UP 2 OCT
            Synthesizers::mKorg->sendProgramChange(34, MIDI); // load b_27
            setup = false;
        }
        break;

    default:
        proceed_to_next_score();
        break;
    }
}