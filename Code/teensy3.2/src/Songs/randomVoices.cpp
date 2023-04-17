#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// RANDOM VOICE /////////////////////////////
// 1: playMidi+CC_Change; 2: change_cc only
void run_randomVoice(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch(Globals::active_song->step)
    {
    case 0:
        Globals::active_song->resetInstruments(); // reset all instruments to "Monitor" mode
        Synthesizers::mKorg->sendProgramChange(int(random(0, 128)), MIDI);
        // notes.push_back(int(random(24, 48)));
        Drumset::snare->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);

        Drumset::kick->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom1->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom2->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 50, -0.1);
        Drumset::standtom->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom2->setup_midi(CC_None, Synthesizers::mKorg, 115, 15, 20, -0.06);

        Globals::active_song->step = 1;
        break;

    case 1: // change CC ("Reflex") + PlayMidi
        if (Globals::active_song->setup)
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

            Globals::active_song->playSingleNote(Synthesizers::mKorg, MIDI);

            Globals::active_song->setup = false;
        }
        break;

    case 2: // change CC only
        if (Globals::active_song->setup)
        {
            // Hardware::footswitch_mode = Experimental;
            Drumset::kick->shuffle_cc(true); // set a random midi CC channel
            Drumset::snare->shuffle_cc(true); // set a random midi CC channel
            Drumset::tom1->shuffle_cc(true); // set a random midi CC channel
            Drumset::tom2->shuffle_cc(true); // set a random midi CC channel
            Drumset::standtom->shuffle_cc(true); // set a random midi CC channel

            // Drumset::snare->setup_midi(dd200_DelayTime, Synthesizers::dd200, 89, 0, -9.96, 0.08);
            Drumset::snare->set_effect(Change_CC);
            Drumset::kick->set_effect(Change_CC);
            Drumset::tom1->set_effect(Change_CC);
            Drumset::tom2->set_effect(Change_CC);
            Drumset::standtom->set_effect(Change_CC);
            Drumset::standtom->set_effect(Change_CC);
            Drumset::tom2->set_effect(Change_CC);
            Drumset::hihat->set_effect(TapTempo);

            Globals::active_song->playSingleNote(Synthesizers::mKorg, MIDI);

            Globals::active_song->setup = false;
        }

        break;

    default: // start over again
        Globals::active_song->step = 1;
        Globals::active_song->setup = true;
        Synthesizers::mKorg->notes[Globals::active_song->notes[Globals::active_song->note_idx]] = false;
        // Globals::active_song->proceed_to_next_score();
        break;
    }
}