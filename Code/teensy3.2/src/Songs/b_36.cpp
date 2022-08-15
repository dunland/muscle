#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_b_36(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        if (setup)
        {
            resetInstruments(); // reset all instruments to "Monitor" mode
            notes = {Note_B5, Note_E5, Note_B6, Note_E6};
            Synthesizers::mKorg->sendProgramChange(85, MIDI); // load b_36

            Drumset::kick->setup_midi(None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::kick->midi_settings.notes.push_back(notes[0]);
            Drumset::kick->midi_settings.active_note = Drumset::kick->midi_settings.notes[0];
            Drumset::kick->set_effect(PlayMidi);

            // TODO: hitting the snare kills supermuscle! fix!!
            Drumset::snare->setup_midi(None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::snare->midi_settings.notes.push_back(notes[1]);
            Drumset::snare->midi_settings.active_note = Drumset::snare->midi_settings.notes[1];
            Drumset::snare->set_effect(PlayMidi);

            Drumset::standtom->setup_midi(None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::standtom->midi_settings.notes.push_back(notes[2]);
            Drumset::standtom->midi_settings.active_note = Drumset::standtom->midi_settings.notes[2];
            Drumset::standtom->set_effect(PlayMidi);

            Drumset::tom1->setup_midi(None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::tom1->midi_settings.notes.push_back(notes[3]);
            Drumset::tom1->midi_settings.active_note = Drumset::tom1->midi_settings.notes[3];
            Drumset::tom1->set_effect(PlayMidi);

            setup = false;
        }

        break;

    default:
        proceed_to_next_score();
        break;
    }
}