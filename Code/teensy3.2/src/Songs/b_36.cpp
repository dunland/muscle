#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_b_36(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->setup)
        {
            Globals::active_song->resetInstruments(); // reset all instruments to "Monitor" mode
            Globals::active_song->notes = {Note_B5, Note_E5, Note_B6, Note_E6};
            Synthesizers::mKorg->sendProgramChange(85, MIDI); // load b_36

            Drumset::kick->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::kick->midi_settings.notes.push_back(Globals::active_song->notes[0]);
            Drumset::kick->midi_settings.active_note = Drumset::kick->midi_settings.notes[0];
            Drumset::kick->set_effect(PlayMidi);

            // TODO: hitting the snare kills supermuscle! fix!!
            Drumset::snare->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::snare->midi_settings.notes.push_back(Globals::active_song->notes[1]);
            Drumset::snare->midi_settings.active_note = Drumset::snare->midi_settings.notes[1];
            Drumset::snare->set_effect(PlayMidi);

            Drumset::standtom->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::standtom->midi_settings.notes.push_back(Globals::active_song->notes[2]);
            Drumset::standtom->midi_settings.active_note = Drumset::standtom->midi_settings.notes[2];
            Drumset::standtom->set_effect(PlayMidi);

            Drumset::tom1->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::tom1->midi_settings.notes.push_back(Globals::active_song->notes[3]);
            Drumset::tom1->midi_settings.active_note = Drumset::tom1->midi_settings.notes[3];
            Drumset::tom1->set_effect(PlayMidi);

            Globals::active_song->setup = false;
        }

        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}