#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

static std::vector<int> list_of_songs = {
    64,  // b.11 + OSC2->Semitone Scale -24 -> +24
    85,  // b.36 z.B. Kick->Filter-Type
    114, // b.73 = noise-trigger
    27,  // b.27
    106, // b.63 mit ARP + hihat-clock
    4,   // A.15
    12   // A.25
};

void run_randomSelect(midi::MidiInterface<HardwareSerial> MIDI)
{
    static const int possible_songs[7] = {64, 85, 114, 27, 106, 4, 12};
    static int selected_song;

    switch (Globals::active_song->get_setup_state())
    {
    case 0:
        Globals::active_song->resetInstruments(); // reset all instruments to "Monitor" mode

        selected_song = possible_songs[int(random(0, 7))];

        Synthesizers::mKorg->sendProgramChange(selected_song, MIDI);

        if (selected_song == 106 || selected_song == 85 || selected_song == 4 || selected_song == 12)
        {
            Synthesizers::mKorg->sendControlChange(mKORG_Arpeggio_onOff, 127, MIDI);
        }

        // notes.push_back(int(random(24, 48)));
        Drumset::snare->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);

        Drumset::kick->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        Drumset::tom1->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        // Drumset::tom2->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 50, -0.1);
        Drumset::standtom->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
        // Drumset::tom2->setup_midi(CC_None, Synthesizers::mKorg, 115, 15, 20, -0.06);

        Globals::active_song->step = 1;
        break;

    case 1: // change CC ("Reflex") + PlayMidi
        if (Globals::active_song->get_setup_state())
        {
            Hardware::footswitch_mode = Increment_Score;

            Drumset::kick->midi_settings.notes.push_back(int(random(0, 128)));
            Drumset::kick->midi_settings.active_note = Drumset::kick->midi_settings.notes[0];

            Drumset::snare->midi_settings.notes.push_back(int(random(12, 72)));
            Drumset::snare->midi_settings.active_note = Drumset::snare->midi_settings.notes[0];

            Drumset::tom1->midi_settings.notes.push_back(int(random(12, 72)));
            Drumset::tom1->midi_settings.active_note = Drumset::tom1->midi_settings.notes[0];

            // Drumset::tom2->midi_settings.notes.push_back(int(random(12, 72)));
            // Drumset::tom2->midi_settings.active_note = Drumset::tom2->midi_settings.notes[0];

            Drumset::standtom->midi_settings.notes.push_back(int(random(12, 72)));
            Drumset::standtom->midi_settings.active_note = Drumset::standtom->midi_settings.notes[0];
            // Drumset::standtom->setup_midi(mKORG_Resonance, Synthesizers::mKorg, 115, 15, 10, -0.6);

            Drumset::kick->set_effect(PlayMidi);
            Drumset::snare->set_effect(PlayMidi);
            Drumset::standtom->set_effect(PlayMidi);
            Drumset::tom1->set_effect(PlayMidi);
            // Drumset::tom2->set_effect(PlayMidi);
            Drumset::hihat->set_effect(TapTempo);
        }
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
            // Drumset::tom2->set_effect(Change_CC);
            Drumset::standtom->set_effect(Change_CC);
            Drumset::standtom->set_effect(Change_CC);
            // Drumset::tom2->set_effect(Change_CC);
            Drumset::hihat->set_effect(TapTempo);

            Synthesizers::mKorg->sendControlChange(mKORG_Sustain, 127, MIDI);
            Globals::active_song->playSingleNote(Synthesizers::mKorg, MIDI);
        }
        Globals::active_song->playSingleNote(Synthesizers::mKorg, MIDI);

        break;

    default: // start over again
        Globals::active_song->step = 1;
        Globals::active_song->setup_state = true;
        Synthesizers::mKorg->notes[Globals::active_song->notes[Globals::active_song->note_idx]] = false;
        // Globals::active_song->proceed_to_next_score();
        break;
    }

    // static int sel_song_int;

    // switch (Globals::active_song->step)
    // {
    // case 0:
    //     if (Globals::active_song->get_setup_state())
    //     {
    //         sel_song_int = list_of_songs.at(random(sizeof(list_of_songs)));
    //     }

    //     switch (sel_song_int)
    //     {
    //     case 85:
    //         // run_b_36(MIDI);
    //         // Globals::active_song->name = "b_36";
    //         // TODO: Globals::active_song = *song; // SONST WIRD SONG::SETUP NICHT AUSGEFÜHRT!
    //         break;

    //     case 64:
    //         run_b_11(MIDI);
    //         Globals::active_song->name = "b_11";
    //         break;

    //     case 114:
    //         run_b_73(MIDI);
    //         Globals::active_song->name = "b_73";
    //         break;

    //     case 27:
    //         run_b_27(MIDI);
    //         Globals::active_song->name = "b_27";
    //         break;

    //     case 106:
    //         run_b_63(MIDI);
    //         Globals::active_song->name = "b_63";
    //         break;

    //     case 4:
    //         run_A_15(MIDI);
    //         Globals::active_song->name = "A_15";
    //         break;

    //     case 12:
    //         run_A_25(MIDI);
    //         Globals::active_song->name = "A_25";
    //         break;

    //     default:
    //         Serial.print("No Song at ");
    //         Serial.println(sel_song_int);
    //         break;
    //     }

    //     break;

    // default:
    //     break;
    // }
}
