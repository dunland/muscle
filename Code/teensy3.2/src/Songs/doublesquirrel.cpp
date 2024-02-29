#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>
#include <settings.h>

//////////////////////////// DOUBLE SQUIRREL /////////////////////////////
// old routine from master thesis presentation
// THIS SONG IS COMPOSED FOR microKORG A.63
// SCORE, stepwise:
// step proceeds if footswitch is pressed (in mode RESET_AND_PROCEED_SCORE) when regularity is high enough
void run_doubleSquirrel() // TODO: make this much more automatic!!
{
    static Song *active_song = Globals::active_song;

    static Instrument *kick = Drumset::kick;
    static Instrument *hihat = Drumset::hihat;
    static Instrument *snare = Drumset::snare;
    static Instrument *crash1 = Drumset::crash1;
    static Instrument *ride = Drumset::ride;
    static Instrument *standtom = Drumset::standtom;
    static Instrument *tom2 = Drumset::tom2;
    //   static int note_idx = -1;
    //   static int rhythmic_iterator;

    switch (active_song->step)
    {

        // case 0: // notesAndEffects_locrianMode
        // {
        //   if (active_song->setup)
        //   {
        //     // Cymbals → random CC (Synthesizers::mKorg)
        //     // drums → playMidi (Volca)

        //     // assign random-cc-effect to cymbals:
        //     crash1->addMidiTarget(None,Synthesizers::mKorg);
        //     crash1->set_effect(Random_CC_Effect);
        //     ride->addMidiTarget(None,Synthesizers::mKorg);
        //     ride->set_effect(Random_CC_Effect);

        //     // set list of notes for kick, snare, tom, standtom
        //     kick->allocateNotesToTarget(locrian_mode);
        //     snare->allocateNotesToTarget(locrian_mode);
        //     tom2->allocateNotesToTarget(locrian_mode);
        //     standtom->allocateNotesToTarget(locrian_mode);

        //     // drums play on Volca:
        //     kick->addMidiTarget(None, volca);
        //     snare->addMidiTarget(None, volca);
        //     tom2->addMidiTarget(None, volca);
        //     standtom->addMidiTarget(None, volca);

        //     // proceed in note lists:
        //     note_idx = (note_idx + 1) % locrian_mode.size();
        //     active_song->note_idx = (active_song->note_idx + 1) % active_song->notes.size();

        //     kick->midi.active_note = kick->midi.notes[note_idx];
        //     kick->set_effect(PlayMidi);

        //     snare->midi.active_note = snare->midi.notes[note_idx] + 12;
        //     snare->set_effect(PlayMidi);

        //     tom2->midi.active_note = tom2->midi.notes[(note_idx + 1) % tom2->midi.notes.size()];
        //     tom2->set_effect(PlayMidi);

        //     standtom->midi.active_note = standtom->midi.notes[(note_idx + 2) % standtom->midi.notes.size()];
        //     standtom->set_effect(PlayMidi);

        //     // add locrian mode
        //     active_song->allocateNotesToTarget({locrian_mode[0], locrian_mode[1], locrian_mode[2]});

        //     // start bass note:
        //     active_song->playSingleNote(Synthesizers::mKorg;

        //     // leave setup:
        //     active_song->setup = false;
        //   }

        //   // change volca, with minimum of 50:
        //   // if (volca->cutoff >= 50 && Globals::current_beat_pos )
        //   // {
        //   // if (active_song->beat_sum.average_smooth > 3)
        //   // {
        //   //   int cutoff_val = min(active_song->beat_sum.average_smooth * 7, 127);
        //   //   volca->sendControlChange(LFO_Rate, cutoff_val;
        //   // }
        //   // }
        // }
        // break;

        // case 1: // cymbalNoteIteration

        //   if (active_song->setup)
        //   {
        //     // add locrian mode
        //     active_song->allocateNotesToTarget({locrian_mode[7], locrian_mode[4], locrian_mode[3], locrian_mode[0]});

        //     // set crash1 to change main note:
        //     crash1->addMidiTarget(None,Synthesizers::mKorg);
        //     crash1->set_effect(MainNoteIteration);

        //     // define interval when to change notes:
        //     // rhythmic_iterator = int(random(32));
        //     // Devtools::print_to_console("rhythmic_iterator = ");
        //     // Devtools::println_to_console(rhythmic_iterator);

        //     // proceed in note lists:
        //     note_idx = (note_idx + 1) % locrian_mode.size();
        //     active_song->note_idx = (active_song->note_idx + 1) % active_song->notes.size();

        //     // start bass note:
        //     active_song->playSingleNote(Synthesizers::mKorg;
        //   }
        //   // active_song->playRhythmicNotes(Synthesizers::mKorg, MIDI, rhythmic_iterator);

        //   break;

    case 0: // init state

        Hardware::footswitch_mode = Reset_Topo_and_Proceed_Score;

        static std::vector<int> locrian_mode = {active_song->notes[0] + 1, active_song->notes[0] + 3, active_song->notes[0] + 5, active_song->notes[0] + 6, active_song->notes[0] + 8, active_song->notes[0] + 11};

        // setup score note seed:
        Devtools::print_to_console("active_song->notes[0] = ");
        Devtools::println_to_console(active_song->notes[0]);
        // set start values for microKORG:

        Synthesizers::mKorg->sendControlChange(mKORG_Cutoff, 50); // sets cc_value and sends MIDI-ControlChange
        Synthesizers::mKorg->sendControlChange(mKORG_Mix_Level_1, 0);
        Synthesizers::mKorg->sendControlChange(mKORG_Mix_Level_2, 127);
        Synthesizers::mKorg->sendControlChange(mKORG_Osc2_tune, 0);
        Synthesizers::mKorg->sendControlChange(mKORG_Osc2_semitone, 39);
        Synthesizers::mKorg->sendControlChange(mKORG_Cutoff, 50);
        Synthesizers::mKorg->sendControlChange(mKORG_Resonance, 13);
        Synthesizers::mKorg->sendControlChange(mKORG_Amplevel, 0);

        active_song->increase_step();

        break;

    case 1:                                  // fade in the synth's amplitude
        if (active_song->get_setup_state()) // score setup is run once and reset when next score step is activated.
        {
            // assign effects to instruments:
            // the hihat will change the allocated (AmpLevel) value on the synth, whenever hit:
            hihat->effect = Change_CC;
            hihat->addMidiTarget(mKORG_Amplevel, Synthesizers::mKorg, 127, 0, 0.65, 0);

            // these instruments do not play a role here. just print out what they do:
            snare->effect = Monitor;
            kick->effect = Monitor;
            tom2->effect = Monitor;
            ride->effect = Monitor;
            crash1->effect = Monitor;
            standtom->effect = Monitor;

            // active_song->playSingleNote(Synthesizers::mKorg; // start playing a bass note on synth
        }

        static int randomNote = int(random(16,72));
        if (Globals::current_beat_pos == 0 && Synthesizers::mKorg->notes[randomNote] == false)
            Synthesizers::mKorg->sendNoteOn(randomNote);

        Devtools::print_to_console("amplevel_val = ");
        Devtools::println_to_console(hihat->midiTargets.back()->cc_val);

        if (hihat->midiTargets.back()->cc_val >= 127)
        {
            active_song->beat_sum.activation_thresh = 0; // score step is ready
        }
        break;

    case 2:
        /* kick -> play note
             snare -> play note
          */

        if (active_song->get_setup_state())
        {
            // assign effects to instruments:
            kick->set_effect(PlayMidi);
            // kick->addMidiTarget(None, Synthesizers::volca, 127, 0, 1, 0.1);
            kick->midiTargets.back()->notes.push_back(active_song->notes[0] + 12 + 7);
            kick->midiTargets.back()->active_note = kick->midiTargets.back()->notes[0];

            Devtools::print_to_console("note for kick is: ");
            Devtools::print_to_console(kick->midiTargets.back()->notes[0]);
            Devtools::print_to_console(" active note:");
            Devtools::println_to_console(kick->midiTargets.back()->active_note);

            snare->set_effect(PlayMidi);
            snare->addMidiTarget(CC_None, Synthesizers::volca, 127, 0, 1, 0.1);
            snare->midiTargets.back()->notes.push_back(active_song->notes[0] + 24);
            snare->midiTargets.back()->active_note = snare->midiTargets.back()->notes[0];

            Devtools::print_to_console("note for snare is: ");
            Devtools::print_to_console(snare->midiTargets.back()->notes[0]);
            Devtools::print_to_console(" active note:");
            Devtools::println_to_console(snare->midiTargets.back()->active_note);
        }
        break;

    case 3:
        static int note_iterator;
        if (active_song->get_setup_state())
        {
            note_iterator = int(random(32));
        }
        active_song->playRhythmicNotes(Synthesizers::mKorg, note_iterator); // random rhythmic beatz

        break;

    case 4:

        // beat_sum -> increase cutoff
        // beat_sum -> fade in OSC1
        // snare, kick, ride, crash = FX
        static float step_factor;

        if (active_song->get_setup_state())
        {
            active_song->beat_sum.activation_thresh = 10;
            step_factor = 127 / active_song->beat_sum.activation_thresh;

            hihat->effect = TapTempo;
            standtom->effect = Monitor;
            snare->effect = Change_CC;
            kick->effect = Change_CC;
            ride->effect = Change_CC;
            crash1->effect = Change_CC;

            // midi channels (do not use any Type twice → smaller/bigger will be ignored..)
            snare->addMidiTarget(mKORG_Osc2_tune, Synthesizers::mKorg, 127, 13, 15, -0.1);
            kick->addMidiTarget(mKORG_Amplevel, Synthesizers::mKorg, 127, 80, -35, 0.1);
            ride->addMidiTarget(mKORG_Resonance, Synthesizers::mKorg, 127, 0, 0.4, -0.1);      // TODO: implement oscillation possibility
            crash1->addMidiTarget(mKORG_Patch_3_Depth, Synthesizers::mKorg, 127, 64, 2, -0.1); // Patch 3 is Pitch on A.63; extends -63-0-63 → 0-64-127
        }

        // change cutoff with overall beat_sum until at max
        static int cutoff_val = 50;

        cutoff_val = max(50, (min(127, active_song->beat_sum.average_smooth * step_factor)));
        Synthesizers::mKorg->sendControlChange(mKORG_Cutoff, cutoff_val);

        // fade in Osc1 slowly
        static int osc1_level;
        osc1_level = min(127, active_song->beat_sum.average_smooth * 4);
        Synthesizers::mKorg->sendControlChange(mKORG_Mix_Level_1, osc1_level);

        break;

    case 5: // increase osc2_tune with snare and osc2_semitone with beat_sum
        static float osc2_semitone_val;

        // active_song->set_ramp(...);

        if (active_song->get_setup_state())
        {
            active_song->beat_sum.activation_thresh = 15;
            snare->effect = Change_CC;
            snare->addMidiTarget(mKORG_Osc2_tune, Synthesizers::mKorg, 64, 0, 1, 0);      // does not decrease
            kick->addMidiTarget(mKORG_DelayDepth, Synthesizers::mKorg, 127, 0, 50, -0.1); // TODO: implement oscillation possibility
            ride->addMidiTarget(mKORG_Cutoff, Synthesizers::mKorg, 127, 13, -0.7, 0.1);   // TODO: implement oscillation possibility
        }

        // increase osc2_semitone with beat_sum until at 0
        osc2_semitone_val = max(0, (min(64, active_song->beat_sum.average_smooth * (127 / 15))));
        osc2_semitone_val = max(0, min(127, osc2_semitone_val));

        Devtools::print_to_console("\n -- Osc2-tune = ");
        Devtools::print_to_console(snare->midiTargets.back()->cc_val);
        Devtools::println_to_console(" --");

        break;

    case 6: // adds new bass note and switches bass once per bar

    {
        static int random_note_change = int(random(32));

        if (active_song->get_setup_state())
        {
            active_song->beat_sum.activation_thresh = 15;

            // cutoff on tom2:
            tom2->effect = Change_CC;
            tom2->addMidiTarget(mKORG_Cutoff, Synthesizers::mKorg, 127, 20, 30, -0.1);
            ride->effect = Monitor;
            snare->effect = Monitor;

            Synthesizers::mKorg->sendControlChange(mKORG_Resonance, 31);
            Synthesizers::mKorg->sendControlChange(mKORG_Cutoff, 28);
            Synthesizers::mKorg->sendControlChange(mKORG_Osc2_tune, 0);
            active_song->add_bassNote(active_song->notes[0] + int(random(6)));
            // active_song->note_change_pos = int(random(8, 16)); // change at a rate between quarter and half notes
        }

        active_song->playRhythmicNotes(Synthesizers::mKorg, random_note_change);
    }
    break;

    case 7: // play melodies on snare, tom2, standtom

    {
        static int random_note_change = int(random(32));
        if (active_song->get_setup_state())
        {
            // active_song->add_bassNote(active_song->notes[0] + int(random(6)));

            snare->effect = PlayMidi;
            snare->midiTargets.back()->active_note = active_song->notes[0] + 24 + 7;
            tom2->effect = PlayMidi;
            tom2->midiTargets.back()->active_note = active_song->notes[0] + 24 + 3;
            standtom->effect = PlayMidi;
            standtom->midiTargets.back()->active_note = active_song->notes[0] + 12 + 4;

            kick->effect = Monitor;
        }

        active_song->playRhythmicNotes(Synthesizers::mKorg, random_note_change);
    }
    break;

        // case 7: // swell effect with notes!
        // break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }

    // vibrate if new score is ready:
    if (Globals::active_song->beat_sum.ready())
    {
        digitalWrite(VIBR, HIGH);
        Devtools::println_to_console("ready to go to next score step! hit footswitch!");
    }
    else
        digitalWrite(VIBR, LOW);
}