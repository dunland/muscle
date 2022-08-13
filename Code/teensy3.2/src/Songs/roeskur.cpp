#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_roeskur(midi::MidiInterface<HardwareSerial> MIDI)
{

    // static Synthesizer *mKorg = Synthesizers::mKorg;
    // static int noteIdx = 0;

    switch (step)
    {
    case 0:
    if (setup)
    {
        Synthesizers::whammy->sendProgramChange(62, MIDI); // HARMONY UP OCT / UP 2 OCT
        setup = false;
    }
        break;

    default:
        proceed_to_next_score();
        break;
    }
}