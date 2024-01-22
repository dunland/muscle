#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>

void run_A_15() // wie randomVoices
{
    switch (Globals::active_song->step)
    {
    case 0:
        Globals::active_song->resetInstruments();  // reset all instruments to "Monitor" mode
        Synthesizers::mKorg->sendProgramChange(4); // A.15
        // notes.push_back(int(random(24, 48)));
        Drumset::snare->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);

        Drumset::kick->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom1->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom2->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 50, -0.1);
        Drumset::standtom->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom2->addMidiTarget(CC_None, Synthesizers::mKorg, 115, 15, 20, -0.06);

        Globals::active_song->increase_step();
        break;

    case 1: // PlayMidi
        if (Globals::active_song->get_setup_state())
        {
            Hardware::footswitch_mode = Increment_Score;

            Drumset::kick->midiTargets.back()->notes.push_back(int(random(0, 128)));
            Drumset::kick->midiTargets.back()->active_note = Drumset::kick->midiTargets.back()->notes[0];

            Drumset::snare->midiTargets.back()->notes.push_back(int(random(12, 72)));
            Drumset::snare->midiTargets.back()->active_note = Drumset::snare->midiTargets.back()->notes[0];

            Drumset::tom1->midiTargets.back()->notes.push_back(int(random(12, 72)));
            Drumset::tom1->midiTargets.back()->active_note = Drumset::tom1->midiTargets.back()->notes[0];

            Drumset::tom2->midiTargets.back()->notes.push_back(int(random(12, 72)));
            Drumset::tom2->midiTargets.back()->active_note = Drumset::tom2->midiTargets.back()->notes[0];

            Drumset::standtom->midiTargets.back()->notes.push_back(int(random(12, 72)));
            Drumset::standtom->midiTargets.back()->active_note = Drumset::standtom->midiTargets.back()->notes[0];
            // Drumset::standtom->addMidiTarget(mKORG_Resonance, Synthesizers::mKorg, 115, 15, 10, -0.6);

            Drumset::kick->set_effect(PlayMidi);
            Drumset::snare->set_effect(PlayMidi);
            Drumset::standtom->set_effect(PlayMidi);
            Drumset::tom1->set_effect(PlayMidi);
            Drumset::tom2->set_effect(PlayMidi);
            Drumset::hihat->set_effect(TapTempo);
        }
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print("playMidi");
        break;

    case 2: // change CC only
        if (Globals::active_song->get_setup_state())
        {
            // Hardware::footswitch_mode = Experimental;
            Drumset::kick->shuffle_cc(Drumset::kick->midiTargets.back(), true);         // set a random midi CC channel
            Drumset::snare->shuffle_cc(Drumset::snare->midiTargets.back(), true);       // set a random midi CC channel
            Drumset::tom1->shuffle_cc(Drumset::tom1->midiTargets.back(), true);         // set a random midi CC channel
            Drumset::tom2->shuffle_cc(Drumset::tom2->midiTargets.back(), true);         // set a random midi CC channel
            Drumset::standtom->shuffle_cc(Drumset::standtom->midiTargets.back(), true); // set a random midi CC channel

            // Drumset::snare->addMidiTarget(dd200_DelayTime, Synthesizers::dd200, 89, 0, -9.96, 0.08);
            Drumset::snare->set_effect(Change_CC);
            Drumset::kick->set_effect(Change_CC);
            Drumset::tom1->set_effect(Change_CC);
            Drumset::tom2->set_effect(Change_CC);
            Drumset::standtom->set_effect(Change_CC);
            Drumset::standtom->set_effect(Change_CC);
            Drumset::tom2->set_effect(Change_CC);
            Drumset::hihat->set_effect(TapTempo);

            Synthesizers::mKorg->sendControlChange(mKORG_Sustain, 127);
            // Globals::active_song->playSingleNote(Synthesizers::mKorg;
        }

        static int randomNote = int(random(16, 72));
        if (Globals::current_beat_pos == 0 && Synthesizers::mKorg->notes[randomNote] == false)
            Synthesizers::mKorg->sendNoteOn(randomNote);

        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print("changeCC");

        break;

    default: // start over again
        Globals::active_song->step = 1;
        Globals::active_song->setup_state = true;
        Synthesizers::mKorg->notes[Globals::active_song->notes[Globals::active_song->note_idx]] = false;
        // proceed_to_next_score();
        break;
    }
}

// TODO: use this callback function!
void leave_A15()
{
    Drumset::kick->midiTargets.pop_back();
    Drumset::snare->midiTargets.pop_back();
    Drumset::tom1->midiTargets.pop_back();
    Drumset::tom2->midiTargets.pop_back();
    Drumset::standtom->midiTargets.pop_back();
}