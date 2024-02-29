#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>

// b.73: heavy tonal noise Trigger
void run_b_73()
{

    switch (Globals::active_song->step)
    {
    case 0:
        Globals::active_song->resetInstruments(); // reset all instruments to "Monitor" mode
        Synthesizers::mKorg->sendProgramChange(114);
        // notes.push_back(int(random(24, 48)));
        Drumset::snare->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);

        Drumset::kick->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom1->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom2->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 50, -0.1);
        Drumset::standtom->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom2->addMidiTarget(CC_None, Synthesizers::mKorg, 115, 15, 20, -0.06);

        Globals::active_song->increase_step();
        break;

    case 1: // Instruments: PlayMidi
        if (Globals::active_song->get_setup_state())
        {
            Hardware::footswitch_mode = Increment_Score;

            Drumset::kick->midiTargets.back()->notes.push_back(int(random(21, 128)));
            Drumset::kick->midiTargets.back()->active_note = Drumset::kick->midiTargets.back()->notes.size() - 1;

            Drumset::snare->midiTargets.back()->notes.push_back(int(random(33, 72)));
            Drumset::snare->midiTargets.back()->active_note = Drumset::snare->midiTargets.back()->notes.size() - 1;

            Drumset::tom1->midiTargets.back()->notes.push_back(int(random(33, 72)));
            Drumset::tom1->midiTargets.back()->active_note = Drumset::tom1->midiTargets.back()->notes.size() - 1;

            Drumset::tom2->midiTargets.back()->notes.push_back(int(random(33, 72)));
            Drumset::tom2->midiTargets.back()->active_note = Drumset::tom2->midiTargets.back()->notes.size() - 1;

            Drumset::standtom->midiTargets.back()->notes.push_back(int(random(33, 72)));
            Drumset::standtom->midiTargets.back()->active_note = Drumset::standtom->midiTargets.back()->notes.size() - 1;
            // Drumset::standtom->addMidiTarget(mKORG_Resonance, Synthesizers::mKorg, 115, 15, 10, -0.6);

            Drumset::kick->set_effect(PlayMidi);
            Drumset::snare->set_effect(PlayMidi);
            Drumset::standtom->set_effect(PlayMidi);
            Drumset::tom1->set_effect(PlayMidi);
            Drumset::tom2->set_effect(PlayMidi);
            Drumset::hihat->set_effect(TapTempo);
        }

        Hardware::lcd->setCursor(0,0);
        Hardware::lcd->print("playMidi");

        break;

    default: // start over again
        Globals::active_song->step = 1; // reset
        Globals::active_song->setup_state = true;
        Synthesizers::mKorg->notes[Globals::active_song->notes[Globals::active_song->note_idx]] = false;
        // proceed_to_next_score();
        break;
    }
}