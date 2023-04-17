#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

// b.73: heavy tonal noise Trigger
void run_b_73(midi::MidiInterface<HardwareSerial> MIDI)
{

    switch (Globals::active_song->step)
    {
    case 0:
        Globals::active_song->resetInstruments(); // reset all instruments to "Monitor" mode
        Synthesizers::mKorg->sendProgramChange(114, MIDI);
        // notes.push_back(int(random(24, 48)));
        Drumset::snare->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);

        Drumset::kick->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom1->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom2->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 50, -0.1);
        Drumset::standtom->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom2->setup_midi(CC_None, Synthesizers::mKorg, 115, 15, 20, -0.06);

        Globals::active_song->step = 1;
        break;

    case 1: // Instruments: PlayMidi
        if (Globals::active_song->get_setup_state())
        {
            Hardware::footswitch_mode = Increment_Score;

            Drumset::kick->midi_settings.notes.push_back(int(random(0, 128)));
            Drumset::kick->midi_settings.active_note = Drumset::kick->midi_settings.notes[0];

            Drumset::snare->midi_settings.notes.push_back(int(random(12, 72)));
            Drumset::snare->midi_settings.active_note = Drumset::snare->midi_settings.notes[0];

            Drumset::tom1->midi_settings.notes.push_back(int(random(12, 72)));
            Drumset::tom1->midi_settings.active_note = Drumset::tom1->midi_settings.notes[0];

            Drumset::tom2->midi_settings.notes.push_back(int(random(12, 72)));
            Drumset::tom2->midi_settings.active_note = Drumset::tom2->midi_settings.notes[0];

            Drumset::standtom->midi_settings.notes.push_back(int(random(12, 72)));
            Drumset::standtom->midi_settings.active_note = Drumset::standtom->midi_settings.notes[0];
            // Drumset::standtom->setup_midi(mKORG_Resonance, Synthesizers::mKorg, 115, 15, 10, -0.6);

            Drumset::kick->set_effect(PlayMidi);
            Drumset::snare->set_effect(PlayMidi);
            Drumset::standtom->set_effect(PlayMidi);
            Drumset::tom1->set_effect(PlayMidi);
            Drumset::tom2->set_effect(PlayMidi);
            Drumset::hihat->set_effect(TapTempo);
        }
        break;

    default: // start over again
        Globals::active_song->step = 1;
        Globals::active_song->setup_state = true;
        Synthesizers::mKorg->notes[Globals::active_song->notes[Globals::active_song->note_idx]] = false;
        // proceed_to_next_score();
        break;
    }
}