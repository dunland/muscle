#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

void run_A_25(midi::MidiInterface<HardwareSerial> MIDI)
{

    switch (Globals::active_song->step)
    {
    case 0: // setup only

        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::mKorg->sendControlChange(mKORG_Arpeggio_onOff, 127, MIDI); // Apreggiator on

            Globals::active_song->resetInstruments(); // reset all instruments to "Monitor" mode
            Synthesizers::mKorg->sendProgramChange(12, MIDI);
            // notes.push_back(int(random(24, 48)));
            Drumset::snare->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);

            Drumset::kick->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::tom1->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::tom2->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 50, -0.1);
            Drumset::standtom->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::tom2->setup_midi(CC_None, Synthesizers::mKorg, 115, 15, 20, -0.06);

            Globals::active_song->increase_step();
        }

        break;

    case 1: //  PlayMidi
        if (Globals::active_song->get_setup_state())
        {
            Hardware::footswitch_mode = Increment_Score;

            Drumset::kick->midi.notes.push_back(int(random(0, 128)));
            Drumset::kick->midi.active_note = Drumset::kick->midi.notes[0];

            Drumset::snare->midi.notes.push_back(int(random(12, 72)));
            Drumset::snare->midi.active_note = Drumset::snare->midi.notes[0];

            Drumset::tom1->midi.notes.push_back(int(random(12, 72)));
            Drumset::tom1->midi.active_note = Drumset::tom1->midi.notes[0];

            Drumset::tom2->midi.notes.push_back(int(random(12, 72)));
            Drumset::tom2->midi.active_note = Drumset::tom2->midi.notes[0];

            Drumset::standtom->midi.notes.push_back(int(random(12, 72)));
            Drumset::standtom->midi.active_note = Drumset::standtom->midi.notes[0];
            // Drumset::standtom->setup_midi(mKORG_Resonance, Synthesizers::mKorg, 115, 15, 10, -0.6);

            Drumset::kick->set_effect(PlayMidi);
            Drumset::snare->set_effect(PlayMidi);
            Drumset::standtom->set_effect(PlayMidi);
            Drumset::tom1->set_effect(PlayMidi);
            Drumset::tom2->set_effect(PlayMidi);
            Drumset::hihat->set_effect(TapTempo);
        }
        Hardware::lcd->setCursor(0,0);
        Hardware::lcd->print("PlayMidi");

        break;

    case 2: // change CC only
        if (Globals::active_song->get_setup_state())
        {
            // Hardware::footswitch_mode = Experimental;
            Drumset::kick->shuffle_cc(true);     // set a random midi CC channel
            Drumset::snare->shuffle_cc(true);    // set a random midi CC channel
            Drumset::tom1->shuffle_cc(true);     // set a random midi CC channel
            Drumset::tom2->shuffle_cc(true);     // set a random midi CC channel
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

            Synthesizers::mKorg->sendControlChange(mKORG_Sustain, 127, MIDI);
            // Globals::active_song->playSingleNote(Synthesizers::mKorg, MIDI);
        }

        static int randomNote = int(random(16,72));
        if (Globals::current_beat_pos == 0 && Synthesizers::mKorg->notes[randomNote] == false)
            Synthesizers::mKorg->sendNoteOn(randomNote, MIDI);

        Hardware::lcd->setCursor(0,0);
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