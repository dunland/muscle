#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_roeskur(midi::MidiInterface<HardwareSerial> MIDI)
{

    // static Synthesizer *mKorg = Synthesizers::mKorg;
    // static int noteIdx = 0;

    switch(Song::step)
    {
    case 0:
    if (Song::setup)
    {
        Synthesizers::whammy->sendProgramChange(62, MIDI); // HARMONY UP OCT / UP 2 OCT
        Song::setup = false;
    }
        break;

    default:
        Song::proceed_to_next_score();
        break;
    }
}