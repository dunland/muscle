#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Score::run_roeskur(midi::MidiInterface<HardwareSerial> MIDI)
{

    static Synthesizer *mKorg = Synthesizers::mKorg;
    static int noteIdx = 0;

    switch (step)
    {
    case 0:
        Synthesizers::whammy->sendProgramChange(63, MIDI); // HARMONY UP OCT / UP 2 OCT
        break;

    case 1: // do nothing
        break;

    default:
        increase_step();
        break;
    }
}