#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// A.72 /////////////////////////////
void Score::run_nanokontrol(midi::MidiInterface<HardwareSerial> MIDI)
{
    // each step:
    // - switch between Timbre 1 and 2
    // - add a note to Score::notes
    // - increase note-index
    // → play latest 4 notes

    switch (step)
    {
    case 0:
        if (setup)
        {
            Hardware::footswitch_mode = Increment_Score;
            Globals::machine_state = NanoKontrol_Test;
            setup = false;
        }
        break;

    default:
        Globals::machine_state = Running;
        proceed_to_next_score();
        break;
    }
}