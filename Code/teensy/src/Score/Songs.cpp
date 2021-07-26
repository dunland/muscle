#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

///////////////////////////////////////////////////////////////////////
/////////////////////////////// SONGS /////////////////////////////////
///////////////////////////////////////////////////////////////////////

//////////////////////////// CONTROL DD200 /////////////////////////////
void Score::run_control_dd200(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        if (setup)
        {
            Drumset::crash1->setup_midi(dd200_DelayTime, Synthesizers::dd200, 127, 0, -10, 0.2); // change
            Drumset::crash1->set_effect(Change_CC);
            Drumset::ride->setup_midi(DelayDepth, Synthesizers::dd200, 127, 0, -3, 0.1); // change
            Drumset::ride->set_effect(Change_CC);
            setup = false;
        }

        // static float val;
        // static float step = 1;
        // val = (val + step);
        // if (val == 0)
        //     Hardware::lcd->clear();
        // // if (millis() > last_change + 1000)
        // // {
        // MIDI.sendControlChange(94, int(val) % 127, 2);
        // MIDI.sendControlChange(17, int(val) % 127, 3);
        // synth->sendControlChange(DelayDepth, int(val) % 127, MIDI);
        // last_change = millis();
        // }

        Hardware::lcd->setCursor(10, 0);
        Hardware::lcd->print(Drumset::crash1->midi_settings.cc_val);
        break;

    default:
        break;
    }
}
//////////////////////////// SATTELSTEIN /////////////////////////////
// THIS SONG IS COMPOSED FOR microKORG A.57
void Score::run_sattelstein(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        if (setup)
        {
            // Synthesizers::mKorg->s
            MIDI.sendProgramChange(38, Synthesizers::mKorg->midi_channel); // selects microKORG Sound A.57
            Hardware::footswitch_mode = Increment_Score;
            setup = false;
        }
        break;

    case 1: // play notes G2 and G3
        if (setup)
        {
            setup = false;
        }

        if (Synthesizers::mKorg->notes[55] == false)
            Synthesizers::mKorg->sendNoteOn(55, MIDI); // play note 55 (G) if it is not playing at the moment
        if (Synthesizers::mKorg->notes[43] == false)
            Synthesizers::mKorg->sendNoteOn(43, MIDI); // play note 43 (G) if it is not playing at the moment
        break;

    case 2: // stop playing notes and leave
        if (setup)
        {
            Synthesizers::mKorg->sendNoteOff(55, MIDI); // play note 55 (G) if it is not playing at the moment
            Synthesizers::mKorg->sendNoteOff(43, MIDI); // play note 43 (G) if it is not playing at the moment
            MIDI.sendRealTime(midi::Stop);

            setup = false;
        }
        increase_step();
        break;

    default:

        proceed_to_next_score();
        break;
    }
}

//////////////////////////// ELEKTROSMOFF /////////////////////////////
void Score::run_elektrosmoff(midi::MidiInterface<HardwareSerial> MIDI)
{
    // THIS SONG IS COMPOSED FOR microKORG A.81
    switch (step)
    {
    case 0: // Vocoder not activated
        if (setup)
        {
            MIDI.sendProgramChange(56, Synthesizers::mKorg->midi_channel); // switches to A.81
            Drumset::snare->set_effect(Monitor);
            Hardware::footswitch_mode = Increment_Score;
            Synthesizers::mKorg->delaydepth = 0;
            setup = false;
        }
        break;

    case 1: // Snare → Vocoder fade in
        if (setup)
        {
            Synthesizers::mKorg->sendControlChange(DelayDepth, 0, MIDI);

            Drumset::snare->midi_settings.active_note = 55;                 // G = 55
            Drumset::snare->setup_midi(Amplevel, Synthesizers::mKorg, 127, 0, 3, -0.002); // changes Gate in Vocoder-Mode
            Drumset::snare->set_effect(Change_CC);
            setup = false;
        }

        if (Drumset::snare->midi_settings.synth->notes[55] == false)
            Drumset::snare->midi_settings.synth->sendNoteOn(55, MIDI); // play note 55 (G) if it is not playing at the moment

        // proceed:
        if (Drumset::crash1->timing.wasHit)
        {
            increase_step();
        }
        break;

    case 2: // Snare → increase delay depth
        if (setup)
        {
            Synthesizers::mKorg->sendControlChange(Amplevel, 127, MIDI);
            Drumset::snare->midi_settings.cc_val = 0;
            Drumset::snare->setup_midi(DelayDepth, Synthesizers::mKorg, 90, 0, 3, -0.002);
            setup = false;
        }
        break;

    case 3: // kill switch
        if (setup)
        {
            Drumset::snare->set_effect(Monitor);
            Synthesizers::mKorg->sendControlChange(Amplevel, 0, MIDI);
            Synthesizers::mKorg->sendControlChange(DelayDepth, 0, MIDI);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0, MIDI);
            setup = false;
        }
        break;

    default:
        proceed_to_next_score();
        break;
    }

    // SCORE END -------------------------------
}

//////////////////////////// EXPERIMENTAL /////////////////////////////
// 1: playMidi+CC_Change; 2: change_cc only
void Score::run_experimental(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        step = 1;
        break;

    case 1: // change CC ("Reflex") + PlayMidi
        if (setup)
        {
            // Drumset::snare->midi_settings.active_note = notes[note_idx] + 18;
            // Drumset::kick->midi_settings.active_note = notes[note_idx] + 6;
            // Drumset::tom2->midi_settings.active_note = notes[note_idx] + 16;
            // Drumset::standtom->midi_settings.active_note = notes[note_idx] + 12;
            Hardware::footswitch_mode = Increment_Score;

            Drumset::kick->set_effect(PlayMidi);
            Drumset::kick->setup_midi(None, Synthesizers::mKorg, 127, 0, 1, 0.1);
            Drumset::kick->shuffle_cc(true); // set a random midi CC channel
            Drumset::kick->midi_settings.notes.push_back(notes[0] + 12 + 7);
            Drumset::kick->midi_settings.active_note = Drumset::kick->midi_settings.notes[0];

            Drumset::snare->set_effect(PlayMidi);
            Drumset::snare->setup_midi(None, Synthesizers::mKorg, 127, 0, 1, 0.1);
            Drumset::snare->shuffle_cc(true); // set a random midi CC channel
            Drumset::snare->midi_settings.notes.push_back(notes[0] + 12 + 12);
            Drumset::snare->midi_settings.active_note = Drumset::snare->midi_settings.notes[0];

            Drumset::tom2->set_effect(PlayMidi);
            Drumset::tom2->setup_midi(None, Synthesizers::mKorg, 127, 0, 1, 0.1);
            Drumset::tom2->shuffle_cc(true); // set a random midi CC channel
            Drumset::tom2->midi_settings.notes.push_back(notes[0] + 12 + 5);
            Drumset::tom2->midi_settings.active_note = Drumset::tom2->midi_settings.notes[0];

            Drumset::standtom->set_effect(PlayMidi);
            Drumset::standtom->setup_midi(None, Synthesizers::mKorg, 127, 0, 1, 0.1);
            Drumset::standtom->shuffle_cc(true); // set a random midi CC channel
            Drumset::standtom->midi_settings.notes.push_back(notes[0] + 12 + 7);
            Drumset::standtom->midi_settings.active_note = Drumset::standtom->midi_settings.notes[0];

            Drumset::kick->set_effect(PlayMidi);
            Drumset::snare->set_effect(PlayMidi);
            Drumset::tom2->set_effect(PlayMidi);
            Drumset::standtom->set_effect(PlayMidi);
            Drumset::hihat->set_effect(TapTempo);
            setup = false;
        }
        break;

    case 2: // change CC only
        if (setup)
        {
            Hardware::footswitch_mode = Experimental;

            Drumset::kick->set_effect(Change_CC);
            Drumset::snare->set_effect(Change_CC);
            Drumset::tom2->set_effect(Change_CC);
            Drumset::standtom->set_effect(Change_CC);
            Drumset::hihat->set_effect(TapTempo);

            notes.push_back(int(random(24, 48)));
            playSingleNote(Synthesizers::mKorg, MIDI);

            setup = false;
        }
        break;

    default: // start over again
        step = 0;
        // proceed_to_next_score();
        break;
    }
}

//////////////////////////// DOUBLE SQUIRREL /////////////////////////////
// old routine from master thesis presentation
// THIS SONG IS COMPOSED FOR microKORG A.63
// SCORE, stepwise:
// step proceeds if footswitch is pressed (in mode RESET_AND_PROCEED_SCORE) when regularity is high enough
void Score::run_doubleSquirrel(midi::MidiInterface<HardwareSerial> MIDI) // TODO: make this much more automatic!!
{
    static Score *active_score = Globals::active_score;

    static Instrument *kick = Drumset::kick;
    static Instrument *hihat = Drumset::hihat;
    static Instrument *snare = Drumset::snare;
    static Instrument *crash1 = Drumset::crash1;
    static Instrument *ride = Drumset::ride;
    static Instrument *standtom = Drumset::standtom;
    static Instrument *tom2 = Drumset::tom2;
    //   static int note_idx = -1;
    //   static int rhythmic_iterator;

    switch (active_score->step)
    {

        // case 0: // notesAndEffects_locrianMode
        // {
        //   if (active_score->setup)
        //   {
        //     // Cymbals → random CC (Synthesizers::mKorg)
        //     // drums → playMidi (Volca)

        //     // assign random-cc-effect to cymbals:
        //     crash1->setup_midi(None,Synthesizers::mKorg);
        //     crash1->set_effect(Random_CC_Effect);
        //     ride->setup_midi(None,Synthesizers::mKorg);
        //     ride->set_effect(Random_CC_Effect);

        //     // set list of notes for kick, snare, tom, standtom
        //     kick->set_notes(locrian_mode);
        //     snare->set_notes(locrian_mode);
        //     tom2->set_notes(locrian_mode);
        //     standtom->set_notes(locrian_mode);

        //     // drums play on Volca:
        //     kick->setup_midi(None, volca);
        //     snare->setup_midi(None, volca);
        //     tom2->setup_midi(None, volca);
        //     standtom->setup_midi(None, volca);

        //     // proceed in note lists:
        //     note_idx = (note_idx + 1) % locrian_mode.size();
        //     active_score->note_idx = (active_score->note_idx + 1) % active_score->notes.size();

        //     kick->midi_settings.active_note = kick->midi_settings.notes[note_idx];
        //     kick->set_effect(PlayMidi);

        //     snare->midi_settings.active_note = snare->midi_settings.notes[note_idx] + 12;
        //     snare->set_effect(PlayMidi);

        //     tom2->midi_settings.active_note = tom2->midi_settings.notes[(note_idx + 1) % tom2->midi_settings.notes.size()];
        //     tom2->set_effect(PlayMidi);

        //     standtom->midi_settings.active_note = standtom->midi_settings.notes[(note_idx + 2) % standtom->midi_settings.notes.size()];
        //     standtom->set_effect(PlayMidi);

        //     // add locrian mode
        //     active_score->set_notes({locrian_mode[0], locrian_mode[1], locrian_mode[2]});

        //     // start bass note:
        //     active_score->playSingleNote(Synthesizers::mKorg, MIDI);

        //     // leave setup:
        //     active_score->setup = false;
        //   }

        //   // change volca, with minimum of 50:
        //   // if (volca->cutoff >= 50 && Globals::current_beat_pos )
        //   // {
        //   // if (active_score->beat_sum.average_smooth > 3)
        //   // {
        //   //   int cutoff_val = min(active_score->beat_sum.average_smooth * 7, 127);
        //   //   volca->sendControlChange(LFO_Rate, cutoff_val, MIDI);
        //   // }
        //   // }
        // }
        // break;

        // case 1: // cymbalNoteIteration

        //   if (active_score->setup)
        //   {
        //     // add locrian mode
        //     active_score->set_notes({locrian_mode[7], locrian_mode[4], locrian_mode[3], locrian_mode[0]});

        //     // set crash1 to change main note:
        //     crash1->setup_midi(None,Synthesizers::mKorg);
        //     crash1->set_effect(MainNoteIteration);

        //     // define interval when to change notes:
        //     // rhythmic_iterator = int(random(32));
        //     // Globals::print_to_console("rhythmic_iterator = ");
        //     // Globals::println_to_console(rhythmic_iterator);

        //     // proceed in note lists:
        //     note_idx = (note_idx + 1) % locrian_mode.size();
        //     active_score->note_idx = (active_score->note_idx + 1) % active_score->notes.size();

        //     // start bass note:
        //     active_score->playSingleNote(Synthesizers::mKorg, MIDI);

        //     // leave setup:
        //     active_score->setup = false;
        //   }

        //   // active_score->playRhythmicNotes(Synthesizers::mKorg, MIDI, rhythmic_iterator);

        //   break;

    case 0: // init state

        Hardware::footswitch_mode = Reset_Topo_and_Proceed_Score;

        static std::vector<int> locrian_mode = {notes[0] + 1, notes[0] + 3, notes[0] + 5, notes[0] + 6, notes[0] + 8, notes[0] + 11};

        // setup score note seed:
        Globals::print_to_console("active_score->notes[0] = ");
        Globals::println_to_console(active_score->notes[0]);
        // set start values for microKORG:

        Synthesizers::mKorg->sendControlChange(Cutoff, 50, MIDI); // sets cc_value and sends MIDI-ControlChange
        Synthesizers::mKorg->sendControlChange(Mix_Level_1, 0, MIDI);
        Synthesizers::mKorg->sendControlChange(Mix_Level_2, 127, MIDI);
        Synthesizers::mKorg->sendControlChange(Osc2_tune, 0, MIDI);
        Synthesizers::mKorg->sendControlChange(Osc2_semitone, 39, MIDI);
        Synthesizers::mKorg->sendControlChange(Cutoff, 50, MIDI);
        Synthesizers::mKorg->sendControlChange(Resonance, 13, MIDI);
        Synthesizers::mKorg->sendControlChange(Amplevel, 0, MIDI);

        active_score->step = 1;

        break;

    case 1:                      // fade in the synth's amplitude
        if (active_score->setup) // score setup is run once and reset when next score step is activated.
        {
            // assign effects to instruments:
            // the hihat will change the allocated (AmpLevel) value on the synth, whenever hit:
            hihat->effect = Change_CC;
            hihat->setup_midi(Amplevel,Synthesizers::mKorg, 127, 0, 0.65, 0);

            // these instruments do not play a role here. just print out what they do:
            snare->effect = Monitor;
            kick->effect = Monitor;
            tom2->effect = Monitor;
            ride->effect = Monitor;
            crash1->effect = Monitor;
            standtom->effect = Monitor;

            active_score->playSingleNote(Synthesizers::mKorg, MIDI); // start playing a bass note on synth
            active_score->setup = false;               // leave setup section
        }

        Globals::print_to_console("amplevel_val = ");
        Globals::println_to_console(hihat->midi_settings.cc_val);

        if (hihat->midi_settings.cc_val >= 127)
        {
            active_score->beat_sum.activation_thresh = 0; // score step is ready
        }
        break;

    case 2:
        /* kick -> play note
             snare -> play note
          */

        if (active_score->setup)
        {
            // assign effects to instruments:
            kick->set_effect(PlayMidi);
            kick->setup_midi(None, Synthesizers::volca, 127, 0, 1, 0.1);
            kick->midi_settings.notes.push_back(active_score->notes[0] + 12 + 7);
            kick->midi_settings.active_note = kick->midi_settings.notes[0];

            Globals::print_to_console("note for kick is: ");
            Globals::print_to_console(kick->midi_settings.notes[0]);
            Globals::print_to_console(" active note:");
            Globals::println_to_console(kick->midi_settings.active_note);

            snare->set_effect(PlayMidi);
            snare->setup_midi(None, Synthesizers::volca, 127, 0, 1, 0.1);
            snare->midi_settings.notes.push_back(active_score->notes[0] + 24);
            snare->midi_settings.active_note = snare->midi_settings.notes[0];

            Globals::print_to_console("note for snare is: ");
            Globals::print_to_console(snare->midi_settings.notes[0]);
            Globals::print_to_console(" active note:");
            Globals::println_to_console(snare->midi_settings.active_note);
            active_score->setup = false;
        }
        break;

    case 3:
        static int note_iterator;
        if (active_score->setup)
        {
            note_iterator = int(random(32));
            active_score->setup = false;
        }
        active_score->playRhythmicNotes(Synthesizers::mKorg, MIDI, note_iterator); // random rhythmic beatz

        break;

    case 4:

        // beat_sum -> increase cutoff
        // beat_sum -> fade in OSC1
        // snare, kick, ride, crash = FX
        static float step_factor;

        if (active_score->setup)
        {
            active_score->beat_sum.activation_thresh = 10;
            step_factor = 127 / active_score->beat_sum.activation_thresh;

            hihat->effect = TapTempo;
            standtom->effect = Monitor;
            snare->effect = Change_CC;
            kick->effect = Change_CC;
            ride->effect = Change_CC;
            crash1->effect = Change_CC;

            // midi channels (do not use any Type twice → smaller/bigger will be ignored..)
            snare->setup_midi(Osc2_tune,Synthesizers::mKorg, 127, 13, 15, -0.1);
            kick->setup_midi(Amplevel,Synthesizers::mKorg, 127, 80, -35, 0.1);
            ride->setup_midi(Resonance,Synthesizers::mKorg, 127, 0, 0.4, -0.1);      // TODO: implement oscillation possibility
            crash1->setup_midi(Patch_3_Depth,Synthesizers::mKorg, 127, 64, 2, -0.1); // Patch 3 is Pitch on A.63; extends -63-0-63 → 0-64-127

            active_score->setup = false;
        }

        // change cutoff with overall beat_sum until at max
        static int cutoff_val = 50;

        cutoff_val = max(50, (min(127, active_score->beat_sum.average_smooth * step_factor)));
       Synthesizers::mKorg->sendControlChange(Cutoff, cutoff_val, MIDI);

        // fade in Osc1 slowly
        static int osc1_level;
        osc1_level = min(127, active_score->beat_sum.average_smooth * 4);
       Synthesizers::mKorg->sendControlChange(Mix_Level_1, osc1_level, MIDI);

        break;

    case 5: // increase osc2_tune with snare and osc2_semitone with beat_sum
        static float osc2_semitone_val;

        // active_score->set_ramp(...);

        if (active_score->setup)
        {
            active_score->beat_sum.activation_thresh = 15;
            snare->effect = Change_CC;
            snare->setup_midi(Osc2_tune,Synthesizers::mKorg, 64, 0, 1, 0);      // does not decrease
            kick->setup_midi(DelayDepth,Synthesizers::mKorg, 127, 0, 50, -0.1); // TODO: implement oscillation possibility
            ride->setup_midi(Cutoff,Synthesizers::mKorg, 127, 13, -0.7, 0.1);   // TODO: implement oscillation possibility
            active_score->setup = false;
        }

        // increase osc2_semitone with beat_sum until at 0
        osc2_semitone_val = max(0, (min(64, active_score->beat_sum.average_smooth * (127 / 15))));
        osc2_semitone_val = max(0, min(127, osc2_semitone_val));

        Globals::print_to_console("\n -- Osc2-tune = ");
        Globals::print_to_console(snare->midi_settings.cc_val);
        Globals::println_to_console(" --");

        break;

    case 6: // adds new bass note and switches bass once per bar

    {
        static int random_note_change = int(random(32));

        if (active_score->setup)
        {
            active_score->beat_sum.activation_thresh = 15;

            // cutoff on tom2:
            tom2->effect = Change_CC;
            tom2->setup_midi(Cutoff,Synthesizers::mKorg, 127, 20, 30, -0.1);
            ride->effect = Monitor;
            snare->effect = Monitor;

           Synthesizers::mKorg->sendControlChange(Resonance, 31, MIDI);
           Synthesizers::mKorg->sendControlChange(Cutoff, 28, MIDI);
           Synthesizers::mKorg->sendControlChange(Osc2_tune, 0, MIDI);
            active_score->add_bassNote(active_score->notes[0] + int(random(6)));
            // active_score->note_change_pos = int(random(8, 16)); // change at a rate between quarter and half notes
            active_score->setup = false;
        }

        active_score->playRhythmicNotes(Synthesizers::mKorg, MIDI, random_note_change);
    }
    break;

    case 7: // play melodies on snare, tom2, standtom

    {
        static int random_note_change = int(random(32));
        if (active_score->setup)
        {
            // active_score->add_bassNote(active_score->notes[0] + int(random(6)));

            snare->effect = PlayMidi;
            snare->midi_settings.active_note = active_score->notes[0] + 24 + 7;
            tom2->effect = PlayMidi;
            tom2->midi_settings.active_note = active_score->notes[0] + 24 + 3;
            standtom->effect = PlayMidi;
            standtom->midi_settings.active_note = active_score->notes[0] + 12 + 4;

            kick->effect = Monitor;
            active_score->setup = false;
        }

        active_score->playRhythmicNotes(Synthesizers::mKorg, MIDI, random_note_change);
    }
    break;

        // case 7: // swell effect with notes!
        // break;

    default:
        proceed_to_next_score();
        break;
    }

    // vibrate if new score is ready:
    if (Globals::active_score->beat_sum.ready())
    {
        digitalWrite(VIBR, HIGH);
        Globals::println_to_console("ready to go to next score step! hit footswitch!");
    }
    else
        digitalWrite(VIBR, LOW);
}