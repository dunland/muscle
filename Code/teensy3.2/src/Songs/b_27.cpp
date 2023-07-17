#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_b_27(midi::MidiInterface<HardwareSerial> MIDI)
{
        Notes notes[3] = {Note_D6, Note_F7, Note_G7};
        switch (Globals::active_song->step)
        {
        case 0:
                if (Globals::active_song->get_setup_state())
                {
                        Synthesizers::mKorg->sendProgramChange(78, MIDI); // load b_27

                        // first initialize notes. otherwise effect cannot be set. THIS IS STUPID TODO: fix this!
                        // Drumset::kick->midi.notes = {Note_D6, Note_F7, Note_G7};
                        // Drumset::snare->midi.notes = {Note_D6, Note_F7, Note_G7};
                        // Drumset::standtom->midi.notes = {Note_D6, Note_F7, Note_G7};
                        // Drumset::tom1->midi.notes = {Note_D6, Note_F7, Note_G7};

                        int idx = random(sizeof(Globals::active_song->notes));
                        Drumset::kick->midi.notes.push_back(Globals::active_song->notes[idx]);
                        Drumset::kick->midi.active_note = notes[idx];
                        idx = random(sizeof(Globals::active_song->notes));
                        Drumset::snare->midi.notes.push_back(Globals::active_song->notes[idx]);
                        Drumset::snare->midi.active_note = notes[idx];
                        idx = random(sizeof(Globals::active_song->notes));
                        Drumset::standtom->midi.notes.push_back(Globals::active_song->notes[idx]);
                        Drumset::standtom->midi.active_note = notes[idx];
                        idx = random(sizeof(Globals::active_song->notes));
                        Drumset::tom1->midi.notes.push_back(Globals::active_song->notes[idx]);
                        Drumset::tom1->midi.active_note = notes[idx];

                        Drumset::snare->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
                        Drumset::standtom->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
                        Drumset::tom1->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);

                        Drumset::snare->set_effect(PlayMidi);
                        Drumset::standtom->set_effect(PlayMidi);
                        Drumset::tom1->set_effect(PlayMidi);
                }

                if (Drumset::kick->timing.wasHit)
                {
                        Synthesizers::mKorg->sendControlChange(mKORG_Osc2_semitone, int(map(-5, -24, 24, 0, 127)), MIDI);
                }

                break;

        case 1:

                if (Globals::active_song->get_setup_state())
                {
                        int idx = random(sizeof(Globals::active_song->notes));
                        Drumset::kick->midi.active_note = notes[idx];
                        idx = random(sizeof(Globals::active_song->notes));
                        Drumset::snare->midi.active_note = notes[idx];

                        idx = random(sizeof(notes));
                        Drumset::standtom->midi.active_note = notes[idx];
                        idx = random(sizeof(Globals::active_song->notes));
                        Drumset::tom1->midi.active_note = notes[idx];
                }

                if (Drumset::kick->timing.wasHit)
                {
                        Synthesizers::mKorg->sendControlChange(mKORG_Osc2_semitone, int(map(-5, -24, 24, 0, 127)), MIDI);
                }

                break;

        default:
                Globals::active_song->step = 1;
                Globals::active_song->setup_state = true;
                break;
        }
}