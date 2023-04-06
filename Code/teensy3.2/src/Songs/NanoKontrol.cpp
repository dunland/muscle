#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// A.72 /////////////////////////////
void run_nanokontrol(midi::MidiInterface<HardwareSerial> MIDI)
{

    switch (Song::step)
    {
    case 0:  // A.43
        if (Song::setup)
        {
            Hardware::footswitch_mode = Increment_Score;
            // Globals::machine_state = NanoKontrol_Test;
            Drumset::snare->setup_midi(mKORG_LFO2_Rate, Synthesizers::mKorg, 127, 30, 18.0, -0.11);
            Drumset::snare->set_effect(Change_CC);
            Song::setup = false;
            Synthesizers::mKorg->sendNoteOn(Song::notes[Song::note_idx], MIDI);
        }
        break;

    default:
        Synthesizers::mKorg->sendNoteOff(Song::notes[Song::note_idx], MIDI);
        Song::note_idx = (Song::note_idx + 1) % sizeof(Song::notes);
        Song::step = 0;
        // Globals::machine_state = Running;
        // Song::proceed_to_next_score();
        break;
    }
}