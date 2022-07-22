#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_alhambra(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        Synthesizers::mKorg->sendProgramChange(34, MIDI); // load b_27
        break;

    default:
        proceed_to_next_score();
        break;
    }
}