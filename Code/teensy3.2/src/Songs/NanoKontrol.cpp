#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// A.72 /////////////////////////////
void run_nanokontrol()
{

    switch (Globals::active_song->step)
    {
    case 0:  // A.43
        if (Globals::active_song->get_setup_state())
        {
            Hardware::footswitch_mode = Increment_Score;
            // Globals::machine_state = NanoKontrol_Test;
            Drumset::snare->setup_midi(mKORG_LFO2_Rate, Synthesizers::mKorg, 127, 30, 18.0, -0.11);
            Drumset::snare->set_effect(Change_CC);

            Synthesizers::mKorg->sendNoteOn(Globals::active_song->notes[Globals::active_song->note_idx]);
        }
        break;

    default:
        Synthesizers::mKorg->sendNoteOff(Globals::active_song->notes[Globals::active_song->note_idx]);
        Globals::active_song->note_idx = (Globals::active_song->note_idx + 1) % sizeof(Globals::active_song->notes);
        Globals::active_song->step = 0;
        // Globals::machine_state = Running;
        // Globals::active_song->proceed_to_next_score();
        break;
    }
}