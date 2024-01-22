#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// A.72 /////////////////////////////
void run_A_72()
{
    // each step:
    // - switch between Timbre 1 and 2
    // - add a note to Score::notes
    // - increase note-index
    // → play latest 4 notes

    // int note_increase = 4; // notes are in turns added by 4 or 5
    static float val;

    switch (Globals::active_song->step)
    {
    case 0: // increase amplitude until max
        if (Globals::active_song->get_setup_state())
        {
            val = 60;

            Globals::active_song->resetInstruments();
            Globals::active_song->notes.clear();
            Globals::active_song->notes.push_back(int(random(12, 24)));

            Hardware::footswitch_mode = Increment_Score;
            Synthesizers::mKorg->sendProgramChange(49); // switches to Voice A.72
            delay(200);
            Synthesizers::mKorg->sendControlChange(mKORG_TimbreSelect, 1); // Select Timbre 2

            Drumset::tom1->set_effect(Change_CC);
            Drumset::tom1->addMidiTarget(mKORG_Cutoff, Synthesizers::mKorg, 127, 29, 3, -0.035);

            Drumset::standtom->set_effect(Change_CC);
            Drumset::standtom->addMidiTarget(mKORG_Resonance, Synthesizers::mKorg, 127, 29, 5, -0.05);

            Drumset::kick->midiTargets.back()->notes.push_back(Globals::active_song->notes[0] + 44);
            Drumset::kick->midiTargets.back()->active_note = Drumset::kick->midiTargets.back()->notes[0];
            Drumset::kick->set_effect(PlayMidi);

            Drumset::snare->midiTargets.back()->notes.push_back(Globals::active_song->notes[0] + 49);
            Drumset::snare->midiTargets.back()->active_note = Drumset::snare->midiTargets.back()->notes[0];
            Drumset::snare->set_effect(PlayMidi);

        }
        if (Globals::current_beat_pos == 0 && Synthesizers::mKorg->notes[Globals::active_song->notes[Globals::active_song->note_idx]] == false)
            Synthesizers::mKorg->sendNoteOn(Globals::active_song->notes[Globals::active_song->note_idx]);

        // increasing amplitude until max:
        val += 0.075;
        Synthesizers::mKorg->sendControlChange(mKORG_Amplevel, int(val));

        Hardware::lcd->setCursor(10, 0);
        Hardware::lcd->print(val);
        Hardware::lcd->setCursor(0,0);
        Hardware::lcd->print("incrAmp:");

        if (Synthesizers::mKorg->midi_values[mKORG_Amplevel] >= 126)
            Globals::active_song->increase_step();
        break;

    case 1:
        if (Globals::active_song->get_setup_state())
        {
            int val = (Synthesizers::mKorg->midi_values[mKORG_TimbreSelect] == 0) ? 127 : 0;
            Synthesizers::mKorg->sendControlChange(mKORG_TimbreSelect, val); // Select Timbre 2
        }

        if (Globals::current_beat_pos == 0 && Synthesizers::mKorg->notes[Globals::active_song->notes[Globals::active_song->note_idx]] == false)
            Synthesizers::mKorg->sendNoteOn(Globals::active_song->notes[Globals::active_song->note_idx]);

        break;

    default:
        // Globals::active_song->step = 1; // reset
        // Globals::active_song->setup_state = true;
        // note_increase = (note_increase == 4) ? 5 : 4;
        // Globals::active_song->notes.push_back(Globals::active_song->notes[Globals::active_song->note_idx] + note_increase);
        // Globals::active_song->note_idx++;
        Synthesizers::mKorg->sendControlChange(mKORG_Arpeggio_onOff, 0); // arp off
        Globals::active_song->proceed_to_next_score();

        break;
    }
}