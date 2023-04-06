#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_b_27(midi::MidiInterface<HardwareSerial> MIDI)
{

    Notes notes[3] = {Note_D6, Note_F7, Note_G7};

    switch (Song::step)
    {
    case 0:
        if (Song::setup)
        {
            Synthesizers::mKorg->sendProgramChange(78, MIDI); // load b_27

            // first initialize notes. otherwise effect cannot be set. THIS IS STUPID TODO: fix this!
            // Drumset::kick->midi_settings.notes = {Note_D6, Note_F7, Note_G7};
            // Drumset::snare->midi_settings.notes = {Note_D6, Note_F7, Note_G7};
            // Drumset::standtom->midi_settings.notes = {Note_D6, Note_F7, Note_G7};
            // Drumset::tom1->midi_settings.notes = {Note_D6, Note_F7, Note_G7};

            int idx = random(sizeof(Song::notes));
            Drumset::kick->midi_settings.notes.push_back(Song::notes[idx]);
            Drumset::kick->midi_settings.active_note = notes[idx];
            idx = random(sizeof(Song::notes));
            Drumset::snare->midi_settings.notes.push_back(Song::notes[idx]);
            Drumset::snare->midi_settings.active_note = notes[idx];
            idx = random(sizeof(Song::notes));
            Drumset::standtom->midi_settings.notes.push_back(Song::notes[idx]);
            Drumset::standtom->midi_settings.active_note = notes[idx];
            idx = random(sizeof(Song::notes));
            Drumset::tom1->midi_settings.notes.push_back(Song::notes[idx]);
            Drumset::tom1->midi_settings.active_note = notes[idx];

            Drumset::snare->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::standtom->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::tom1->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);

            Drumset::snare->set_effect(PlayMidi);
            Drumset::standtom->set_effect(PlayMidi);
            Drumset::tom1->set_effect(PlayMidi);

            Song::setup = false;
        }

        if (Drumset::kick->timing.wasHit)
        {
            Synthesizers::mKorg->sendControlChange(mKORG_Osc2_semitone, int(map(-5, -24, 24, 0, 127)), MIDI);
        }

        break;

    case 1:
        if (Song::setup)
        {
            int idx = random(sizeof(Song::notes));
            Drumset::kick->midi_settings.active_note = notes[idx];
            idx = random(sizeof(Song::notes));
            Drumset::snare->midi_settings.active_note = notes[idx];
            Song::setup = false;
            idx = random(sizeof(Song::notes));
            Drumset::standtom->midi_settings.active_note = notes[idx];
            idx = random(sizeof(Song::notes));
            Drumset::tom1->midi_settings.active_note = notes[idx];
        }

        if (Drumset::kick->timing.wasHit)
        {
            Synthesizers::mKorg->sendControlChange(mKORG_Osc2_semitone, int(map(-5, -24, 24, 0, 127)), MIDI);
        }

        break;

    default:
        Song::step = 1;
        Song::setup = true;
        break;
    }
}