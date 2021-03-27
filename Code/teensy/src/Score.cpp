#include <Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>


/////////////////////////////////// SETUP FUNCTIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void Score::set_notes(std::vector<int> list)
{
    // clear notes list:
    notes.clear();

    // add notes from list:
    for (uint8_t i = 0; i < list.size(); i++)
    {
        notes.push_back(list[i]);
    }

    // print list:
    Globals::print_to_console("Score::notes:");
    for (uint8_t i = 0; i < notes.size(); i++)
    {
        Globals::print_to_console(" ");
        Globals::print_to_console(notes[i]);
    }
    Globals::println_to_console("");
}

void Score::set_step_function(int trigger_step, Instrument *instrument, EffectsType effect_)
{
}

void Score::increase_step()
{
    step++;
    setup = true;
}

void Score::proceed_to_next_score()
{
	Globals::println_to_console("End of score reached! Proceeding to next score!");
	// proceed to next score in list:
	Globals::active_score_pointer = (Globals::active_score_pointer + 1) % Globals::score_list.size();
	// ...and begin at step 0:
	Globals::score_list[Globals::active_score_pointer]->step = 0;
	Globals::score_list[Globals::active_score_pointer]->setup = true;
	Globals::active_score = Globals::score_list[Globals::active_score_pointer];

    Hardware::lcd->clear();

}

//////////////////////////////////// MUSICAL FUNCTIONS //////////////////////////
void Score::add_bassNote(int note)
{
    notes.push_back(note);
    Globals::print_to_console("note ");
    Globals::print_to_console(note);
    Globals::print_to_console(" has been added to Score::notes [ ");
    for (uint8_t i = 0; i < notes.size(); i++)
    {
        Globals::print_to_console(notes[i]);
        Globals::print_to_console(" ");
    }
    Globals::println_to_console("]");
}


///////////////////////////////////////////////////////////////////////
/////////////////////////// STANDARD RUN //////////////////////////////
///////////////////////////////////////////////////////////////////////
void Score::run(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI)
{
    if (name == "elektrosmoff")
    {
        run_elektrosmoff(synth, MIDI);
    }
    else if (name == "doubleSquirrel")
    {
        // run_doubleSquirrel(Synthesizer *mKorg, Synthesizer *volca, midi::MidiInterface<HardwareSerial> MIDI);
        Globals::println_to_console("running 'doubleSquirrel' -- but nothing to do yet..");
        run_doubleSquirrel(MIDI, synth, synth);
    }
    else if (name == "experimental")
    {
        // run_experimental(Synthesizer* mKorg, Synthesizer* volca);
        Globals::println_to_console("running 'experimenal' -- but nothing to do yet..");
        run_experimental(synth, synth);
    }
}

///////////////////////////////////////////////////////////////////////
/////////////////////////////// SONGS /////////////////////////////////
///////////////////////////////////////////////////////////////////////


//////////////////////////// ELEKTROSMOFF /////////////////////////////
void Score::run_elektrosmoff(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI)
{
    // THIS SONG IS COMPOSED FOR microKORG A.81
    switch (step)
    {
    case 0:
        /* vocoder not activated */
        break;

    case 1: // Snare → Vocoder fade in
        if (setup)
        {
            Drumset::snare->set_effect(Monitor);
            synth->delaydepth = 0;

            Drumset::snare->midi_settings.active_note = 55;                 // G = 55
            Drumset::snare->setup_midi(Amplevel, synth, 127, 0, 3, -0.002); // changes Gate in Vocoder-Mode (hopefully)
            setup = false;
        }

        Drumset::snare->set_effect(Change_CC);
        if (Drumset::snare->midi_settings.synth->notes[55] == false)
            Drumset::snare->midi_settings.synth->sendNoteOn(55, MIDI); // play note 55 (G) if it is not playing at the moment

        // proceed:
        if (Drumset::crash1->timing.wasHit)
        {
            step++;
        }
        break;

    case 2: // Snare → increas delay depth
        if (setup)
        {
            synth->amplevel = 127; // keep amp at maxLevel
            Drumset::snare->setup_midi(DelayDepth, synth, 127, 0, 3, -0.002);
        }

    default:

        proceed_to_next_score();
        break;
    }

    // SCORE END -------------------------------
}

//////////////////////////// EXPERIMENTAL /////////////////////////////
// 1: playMidi+CC_Change; 2: change_cc only
void Score::run_experimental(Synthesizer *mKorg, Synthesizer *volca)
{
    switch (step)
    {
    case 1: // change CC ("Reflex") + PlayMidi
        if (setup)
        {
            // Drumset::snare->midi_settings.active_note = notes[note_idx] + 18;
            // Drumset::kick->midi_settings.active_note = notes[note_idx] + 6;
            // Drumset::tom2->midi_settings.active_note = notes[note_idx] + 16;
            // Drumset::standtom->midi_settings.active_note = notes[note_idx] + 12;

            Drumset::kick->set_effect(PlayMidi);
            Drumset::kick->setup_midi(None, mKorg, 127, 0, 1, 0.1);
            Drumset::kick->shuffle_cc(); // set a random midi CC channel
            Drumset::kick->midi_settings.notes.push_back(notes[0] + 12 + 7);
            Drumset::kick->midi_settings.active_note = Drumset::kick->midi_settings.notes[0];

            Drumset::snare->set_effect(PlayMidi);
            Drumset::snare->setup_midi(None, mKorg, 127, 0, 1, 0.1);
            Drumset::snare->shuffle_cc(); // set a random midi CC channel
            Drumset::snare->midi_settings.notes.push_back(notes[0] + 12 + 12);
            Drumset::snare->midi_settings.active_note = Drumset::snare->midi_settings.notes[0];

            Drumset::tom2->set_effect(PlayMidi);
            Drumset::tom2->setup_midi(None, mKorg, 127, 0, 1, 0.1);
            Drumset::tom2->shuffle_cc(); // set a random midi CC channel
            Drumset::tom2->midi_settings.notes.push_back(notes[0] + 12 + 5);
            Drumset::tom2->midi_settings.active_note = Drumset::tom2->midi_settings.notes[0];

            Drumset::standtom->set_effect(PlayMidi);
            Drumset::standtom->setup_midi(None, mKorg, 127, 0, 1, 0.1);
            Drumset::standtom->shuffle_cc(); // set a random midi CC channel
            Drumset::standtom->midi_settings.notes.push_back(notes[0] + 12 + 7);
            Drumset::standtom->midi_settings.active_note = Drumset::standtom->midi_settings.notes[0];

            Hardware::FOOTSWITCH_MODE = Hardware::EXPERIMENTAL;
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
            Hardware::FOOTSWITCH_MODE = Hardware::EXPERIMENTAL;
            Drumset::kick->set_effect(Change_CC);
            Drumset::snare->set_effect(Change_CC);
            Drumset::tom2->set_effect(Change_CC);
            Drumset::standtom->set_effect(Change_CC);
            Drumset::hihat->set_effect(TapTempo);
            setup = false;
        }
        break;

    default:
        proceed_to_next_score();
        break;
    }
}

//////////////////////////// DOUBLE SQUIRREL /////////////////////////////
// old routine from master thesis presentation:
void Score::run_doubleSquirrel(midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *mKorg, Synthesizer *volca) // TODO: make this much more automatic!!
{
    Score *active_score = Globals::active_score;
    
    Instrument *kick = Drumset::kick;
    Instrument *hihat = Drumset::hihat;
    Instrument *snare = Drumset::snare;
    Instrument *crash1 = Drumset::crash1;
    Instrument *ride = Drumset::ride;
    Instrument *standtom = Drumset::standtom;
    Instrument *tom2 = Drumset::tom2;
    //   static int note_idx = -1;
    //   static int rhythmic_iterator;

    switch (active_score->step)
    {

        // case 0: // notesAndEffects_locrianMode
        // {
        //   if (active_score->setup)
        //   {
        //     // Cymbals → random CC (mKorg)
        //     // drums → playMidi (Volca)

        //     // assign random-cc-effect to cymbals:
        //     crash1->setup_midi(None, mKorg);
        //     crash1->set_effect(Random_CC_Effect);
        //     ride->setup_midi(None, mKorg);
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
        //     active_score->playSingleNote(mKorg, MIDI);

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
        //     crash1->setup_midi(None, mKorg);
        //     crash1->set_effect(MainNoteIteration);

        //     // define interval when to change notes:
        //     // rhythmic_iterator = int(random(32));
        //     // Globals::print_to_console("rhythmic_iterator = ");
        //     // Globals::println_to_console(rhythmic_iterator);

        //     // proceed in note lists:
        //     note_idx = (note_idx + 1) % locrian_mode.size();
        //     active_score->note_idx = (active_score->note_idx + 1) % active_score->notes.size();

        //     // start bass note:
        //     active_score->playSingleNote(mKorg, MIDI);

        //     // leave setup:
        //     active_score->setup = false;
        //   }

        //   // active_score->playRhythmicNotes(mKorg, MIDI, rhythmic_iterator);

        //   break;

    case 0: // init state
        // setup score note seed:
        Globals::print_to_console("active_score->notes[0] = ");
        Globals::println_to_console(active_score->notes[0]);
        // set start values for microKORG:

        mKorg->sendControlChange(Cutoff, 50, MIDI); // sets cc_value and sends MIDI-ControlChange
        mKorg->sendControlChange(Mix_Level_1, 0, MIDI);
        mKorg->sendControlChange(Mix_Level_2, 127, MIDI);
        mKorg->sendControlChange(Osc2_tune, 0, MIDI);
        mKorg->sendControlChange(Osc2_semitone, 39, MIDI);
        mKorg->sendControlChange(Cutoff, 50, MIDI);
        mKorg->sendControlChange(Resonance, 13, MIDI);
        mKorg->sendControlChange(Amplevel, 0, MIDI);

        active_score->step = 1;

        break;

    case 1:                      // fade in the synth's amplitude
        if (active_score->setup) // score setup is run once and reset when next score step is activated.
        {
            // assign effects to instruments:
            // the hihat will change the allocated (AmpLevel) value on the synth, whenever hit:
            hihat->effect = Change_CC;
            hihat->setup_midi(Amplevel, mKorg, 127, 0, 0.65, 0);

            // these instruments do not play a role here. just print out what they do:
            snare->effect = Monitor;
            kick->effect = Monitor;
            tom2->effect = Monitor;
            ride->effect = Monitor;
            crash1->effect = Monitor;
            standtom->effect = Monitor;

            active_score->playSingleNote(mKorg, MIDI); // start playing a bass note on synth
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
            kick->setup_midi(None, volca, 127, 0, 1, 0.1);
            kick->midi_settings.notes.push_back(active_score->notes[0] + 12 + 7);
            kick->midi_settings.active_note = kick->midi_settings.notes[0];

            Globals::print_to_console("note for kick is: ");
            Globals::print_to_console(kick->midi_settings.notes[0]);
            Globals::print_to_console(" active note:");
            Globals::println_to_console(kick->midi_settings.active_note);

            snare->set_effect(PlayMidi);
            snare->setup_midi(None, volca, 127, 0, 1, 0.1);
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
        active_score->playRhythmicNotes(mKorg, MIDI, note_iterator); // random rhythmic beatz

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
            snare->setup_midi(Osc2_tune, mKorg, 127, 13, 15, -0.1);
            kick->setup_midi(Amplevel, mKorg, 127, 80, -35, 0.1);
            ride->setup_midi(Resonance, mKorg, 127, 0, 0.4, -0.1);      // TODO: implement oscillation possibility
            crash1->setup_midi(Patch_3_Depth, mKorg, 127, 64, 2, -0.1); // Patch 3 is Pitch on A.63; extends -63-0-63 → 0-64-127

            active_score->setup = false;
        }

        // change cutoff with overall beat_sum until at max
        static int cutoff_val = 50;

        cutoff_val = max(50, (min(127, active_score->beat_sum.average_smooth * step_factor)));
        mKorg->sendControlChange(Cutoff, cutoff_val, MIDI);

        // fade in Osc1 slowly
        static int osc1_level;
        osc1_level = min(127, active_score->beat_sum.average_smooth * 4);
        mKorg->sendControlChange(Mix_Level_1, osc1_level, MIDI);

        break;

    case 5: // increase osc2_tune with snare and osc2_semitone with beat_sum
        static float osc2_semitone_val;

        // active_score->set_ramp(...);

        if (active_score->setup)
        {
            active_score->beat_sum.activation_thresh = 15;
            snare->effect = Change_CC;
            snare->setup_midi(Osc2_tune, mKorg, 64, 0, 1, 0);      // does not decrease
            kick->setup_midi(DelayDepth, mKorg, 127, 0, 50, -0.1); // TODO: implement oscillation possibility
            ride->setup_midi(Cutoff, mKorg, 127, 13, -0.7, 0.1);   // TODO: implement oscillation possibility
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
            tom2->setup_midi(Cutoff, mKorg, 127, 20, 30, -0.1);
            ride->effect = Monitor;
            snare->effect = Monitor;

            mKorg->sendControlChange(Resonance, 31, MIDI);
            mKorg->sendControlChange(Cutoff, 28, MIDI);
            mKorg->sendControlChange(Osc2_tune, 0, MIDI);
            active_score->add_bassNote(active_score->notes[0] + int(random(6)));
            // active_score->note_change_pos = int(random(8, 16)); // change at a rate between quarter and half notes
            active_score->setup = false;
        }

        active_score->playRhythmicNotes(mKorg, MIDI, random_note_change);
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

        active_score->playRhythmicNotes(mKorg, MIDI, random_note_change);
    }
    break;

        //     // case 7: // swell effect with notes!
        //     // break;

    default:
        proceed_to_next_score();
        break;
    }
}

///////////////////////////////////////////////////////////////////////
////////////////////////////// MODES //////////////////////////////////
///////////////////////////////////////////////////////////////////////

// play note, repeatedly:
void Score::playRhythmicNotes(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI, int rhythmic_iterator) // initiates a continuous bass note from score
{
    if (rhythmic_iterator != 0)
        note_change_pos = rhythmic_iterator;

    if ((Globals::current_beat_pos + 1) % note_change_pos == 0)
    {
        // play note
        synth->sendNoteOff(notes[note_idx], MIDI);
        synth->sendNoteOn(notes[note_idx], MIDI);
        Globals::print_to_console("\tplaying Score::note:");
        Globals::println_to_console(notes[note_idx]);

        // change note
        if (notes.size() > 1)
        {
            note_idx++;
            if (note_idx > int(notes.size()) - 1)
                note_idx = 0;
            //  = (note_idx + 1) % notes.size(); // iterate through the bass notes
            Globals::print_to_console("\tnote_idx = ");
            Globals::println_to_console(note_idx);
        }
    }
    // else

    // if (Globals::current_beat_pos == 0) // at beginninng of each bar
    // {
    //     Globals::print_to_console("\tnotes.size() = ");
    //     Globals::println_to_console(int(notes.size()));
    // }
}

// play note only once (turn on never off):
void Score::playSingleNote(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI) // initiates a continuous bass note from score
{
    if (notes.size() > 0)
        synth->sendNoteOn(notes[note_idx], MIDI);
    else
        Globals::println_to_console("cannot play MIDI note, because Score::notes is empty.");
}

void Score::envelope_cutoff(Synthesizer *synth, TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI)
{
    int cutoff_val = topography->a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height

    cutoff_val = max(20, cutoff_val);  // must be at least 20
    cutoff_val = min(cutoff_val, 127); // must not be greater than 127
    synth->sendControlChange(Cutoff, cutoff_val, MIDI);
}

void Score::envelope_volume(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth)
{
    int amp_val = topography->a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    // amp_val = max(0, amp_val);                                    // must be at least 0
    amp_val = min(amp_val, 127); // must not be greater than 127
    synth->sendControlChange(Amplevel, amp_val, MIDI);
}

void Score::crazyDelays(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth)
{
    int delaytime = instrument->topography.a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    delaytime = min(delaytime, 127);                                               // must not be greater than 127
    synth->sendControlChange(DelayTime, delaytime, MIDI);
}

// void Score::set_ramp(midi::MidiInterface<HardwareSerial> MIDI, CC_Type cc_type, MIDI_Instrument midi_instr, int start_value, int end_value, int duration)
// {
//     static boolean ramp_start = true;
//     static int value;
//     static unsigned long start_time;
//     static float factor;

//     if (ramp_start)
//     {
//         value = start_value;
//         start_time = millis();
//         factor = (start_value - end_value) / duration;
//         ramp_start = false;
//     }

//     MIDI.sendControlChange(cc_type, value, midi_instr);
//     value += factor;

//     if (millis() > start_time + duration)
//     {

//     }
// }
