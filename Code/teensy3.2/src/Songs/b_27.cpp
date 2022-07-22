#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_b_27(midi::MidiInterface<HardwareSerial> MIDI)
{

    Notes notes[3] = {Note_D6, Note_F7, Note_G7};

    switch (step)
    {
    case 0:
        if (setup)
        {
            Synthesizers::mKorg->sendProgramChange(78, MIDI); // load b_27

            // first initialize notes. otherwise effect cannot be set. THIS IS STUPID TODO: fix this!
            // Drumset::kick->midi_settings.notes = {Note_D6, Note_F7, Note_G7};
            // Drumset::snare->midi_settings.notes = {Note_D6, Note_F7, Note_G7};
            // Drumset::standtom->midi_settings.notes = {Note_D6, Note_F7, Note_G7};
            // Drumset::tom1->midi_settings.notes = {Note_D6, Note_F7, Note_G7};

            int idx = random(sizeof(notes));
            Drumset::kick->midi_settings.notes.push_back(notes[idx]);
            Drumset::kick->midi_settings.active_note = notes[idx];
            idx = random(sizeof(notes));
            Drumset::snare->midi_settings.notes.push_back(notes[idx]);
            Drumset::snare->midi_settings.active_note = notes[idx];
            idx = random(sizeof(notes));
            Drumset::standtom->midi_settings.notes.push_back(notes[idx]);
            Drumset::standtom->midi_settings.active_note = notes[idx];
            idx = random(sizeof(notes));
            Drumset::tom1->midi_settings.notes.push_back(notes[idx]);
            Drumset::tom1->midi_settings.active_note = notes[idx];

            Drumset::snare->setup_midi(None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::standtom->setup_midi(None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::tom1->setup_midi(None, Synthesizers::mKorg, 127, 0, 10, -0.1);

            Drumset::snare->set_effect(PlayMidi);
            Drumset::standtom->set_effect(PlayMidi);
            Drumset::tom1->set_effect(PlayMidi);

            setup = false;
        }

        if (Drumset::kick->timing.wasHit)
        {
            Synthesizers::mKorg->sendControlChange(mKORG_Osc2_semitone, int(map(-5, -24, 24, 0, 127)), MIDI);
        }

        break;

    case 1:
        if (setup)
        {
            int idx = random(sizeof(notes));
            Drumset::kick->midi_settings.active_note = notes[idx];
            idx = random(sizeof(notes));
            Drumset::snare->midi_settings.active_note = notes[idx];
            setup = false;
            idx = random(sizeof(notes));
            Drumset::standtom->midi_settings.active_note = notes[idx];
            idx = random(sizeof(notes));
            Drumset::tom1->midi_settings.active_note = notes[idx];
        }

        if (Drumset::kick->timing.wasHit)
        {
            Synthesizers::mKorg->sendControlChange(mKORG_Osc2_semitone, int(map(-5, -24, 24, 0, 127)), MIDI);
        }

        break;

    default:
        step = 1;
        setup = true;
        break;
    }
}