#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// A.72 /////////////////////////////
void Song::run_nanokontrol(midi::MidiInterface<HardwareSerial> MIDI)
{

    switch (step)
    {
    case 0:  // A.43
        if (setup)
        {
            Hardware::footswitch_mode = Increment_Score;
            // Globals::machine_state = NanoKontrol_Test;
            Drumset::snare->setup_midi(mKORG_LFO2_Rate, Synthesizers::mKorg, 127, 30, 18.0, -0.11);
            Drumset::snare->set_effect(Change_CC);
            setup = false;
            Synthesizers::mKorg->sendNoteOn(notes[note_idx], MIDI);
        }
        break;

    default:
        Synthesizers::mKorg->sendNoteOff(notes[note_idx], MIDI);
        note_idx = (note_idx + 1) % sizeof(notes);
        step = 0;
        // Globals::machine_state = Running;
        // proceed_to_next_score();
        break;
    }
}