#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_alhambra(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (Song::step)
    {
    case 0:
        if (Song::setup)
        {
            Synthesizers::whammy->sendProgramChange(0, MIDI); // HARMONY UP 2 OCT
            Synthesizers::mKorg->sendProgramChange(34, MIDI); // load b_27
            Song::setup = false;
        }
        break;

    default:
        Song::proceed_to_next_score();
        break;
    }
}