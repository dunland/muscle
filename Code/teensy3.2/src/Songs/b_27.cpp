#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_b_27()
{
        Notes notes[3] = {Note_D6, Note_F7, Note_G7};
        switch (Globals::active_song->step)
        {
        case 0: // setup only
                if (Globals::active_song->get_setup_state())
                {
                        Synthesizers::mKorg->sendProgramChange(78); // load b_27

                        // first initialize notes. otherwise effect cannot be set. THIS IS STUPID TODO: fix this!
                        // Drumset::kick->midi.notes = {Note_D6, Note_F7, Note_G7};
                        // Drumset::snare->midi.notes = {Note_D6, Note_F7, Note_G7};
                        // Drumset::standtom->midi.notes = {Note_D6, Note_F7, Note_G7};
                        // Drumset::tom1->midi.notes = {Note_D6, Note_F7, Note_G7};

                        int idx = random(sizeof(notes));
                        Drumset::snare->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
                        Drumset::standtom->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
                        Drumset::tom1->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);

                        Drumset::kick->midiTargets.back()->notes.push_back(notes[idx]);
                        Drumset::kick->midiTargets.back()->active_note = notes[idx];
                        idx = random(sizeof(notes));
                        Drumset::snare->midiTargets.back()->notes.push_back(notes[idx]);
                        Drumset::snare->midiTargets.back()->active_note = notes[idx];
                        idx = random(sizeof(notes));
                        Drumset::standtom->midiTargets.back()->notes.push_back(notes[idx]);
                        Drumset::standtom->midiTargets.back()->active_note = notes[idx];
                        idx = random(sizeof(notes));
                        Drumset::tom1->midiTargets.back()->notes.push_back(notes[idx]);
                        Drumset::tom1->midiTargets.back()->active_note = notes[idx];

                        Drumset::snare->set_effect(PlayMidi);
                        Drumset::standtom->set_effect(PlayMidi);
                        Drumset::tom1->set_effect(PlayMidi);
                }

                if (Globals::current_beat_pos == 0 && Synthesizers::mKorg->notes[Drumset::snare->midiTargets.back()->active_note - 12] == false)
                        Synthesizers::mKorg->sendNoteOn(Drumset::snare->midiTargets.back()->active_note - 12);

                if (Drumset::kick->timing.wasHit)
                {
                        Synthesizers::mKorg->sendControlChange(mKORG_Osc2_semitone, int(random(0, 127)));
                }

                Hardware::lcd->setCursor(0, 0);
                Hardware::lcd->print("kickOSC2");

                break;

        case 1: // change notes -- kick resets mKORG OSC2semitone

                if (Globals::active_song->get_setup_state())
                {
                        int idx = random(sizeof(notes));
                        Drumset::kick->midiTargets.back()->active_note = notes[idx];
                        idx = random(sizeof(notes));
                        Drumset::snare->midiTargets.back()->active_note = notes[idx];

                        idx = random(sizeof(notes));
                        Drumset::standtom->midiTargets.back()->active_note = notes[idx];
                        idx = random(sizeof(notes));
                        Drumset::tom1->midiTargets.back()->active_note = notes[idx];
                }

                if (Drumset::kick->timing.wasHit)
                {
                        Synthesizers::mKorg->sendControlChange(mKORG_Osc2_semitone, int(random(0, 127)));
                }

                Hardware::lcd->setCursor(0, 0);
                Hardware::lcd->print("kickOSC2");

                break;

        default:
                Globals::active_song->step = 1; // reset
                Globals::active_song->setup_state = true;
                break;
        }
}