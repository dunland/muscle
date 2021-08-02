#include <Score/Score.h>
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
    Serial.println("step");
    Serial.println(step);
    Hardware::lcd->clear();
}

// proceed to step 0 of next score and set all instruments effects to "Monitor":
void Score::proceed_to_next_score()
{
    // proceed to next score in list:
    Globals::active_score_pointer = (Globals::active_score_pointer + 1) % Globals::score_list.size();
    Globals::active_score = Globals::score_list[Globals::active_score_pointer];
    // ...and begin at step 0:
    Globals::active_score->step = 0;
    Globals::active_score->setup = true;

    for (auto &instrument : Drumset::instruments)
        instrument->effect = Monitor;

    for (auto &synth : Synthesizers::synths)
    {
        for (int note = 0; note < 127; note++)
        {
            synth->notes[note] = false;
        }
    }

    Hardware::lcd->clear();
}

// set tempo of the score:
void Score::setTempoRange(int min_tempo_, int max_tempo_)
{
    tempo.min_tempo = min_tempo_;
    tempo.max_tempo = max_tempo_;
}

// reset all instruments to "Monitor" mode
void Score::resetInstruments()
{
    for (auto &drum : Drumset::instruments)
    {
        drum->effect = Monitor;
    }
}

///////////////////////////////////////////////////////////////////////
/////////////////////////// STANDARD RUN //////////////////////////////
///////////////////////////////////////////////////////////////////////
void Score::run(midi::MidiInterface<HardwareSerial> MIDI)
{
    if (name == "elektrosmoff")
    {
        run_elektrosmoff(MIDI);
    }
    else if (name == "doubleSquirrel")
    {
        run_doubleSquirrel(MIDI);
    }
    else if (name == "randomVoice")
    {
        run_randomVoice(MIDI);
    }
    else if (name == "monitoring")
    {
        run_monitoring(MIDI);
    }
    else if (name == "sattelstein")
    {
        run_sattelstein(MIDI);
    }
    else if (name == "control_dd200")
    {
        run_control_dd200(MIDI);
    }
    else if (name == "A.72")
    {
        run_a72(MIDI);
    }
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

// ----------------- play note only once (turn on never off):
void Score::playSingleNote(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI) // initiates a continuous bass note from score
{
    if (notes.size() > 0)
    {
        if (synth->notes[note_idx] == false)
            synth->sendNoteOn(notes[note_idx], MIDI);
    }
    else
        Globals::println_to_console("cannot play MIDI note, because Score::notes is empty.");
}

// play last 3 notes in list:
void Score::playLastThreeNotes(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI)
{
    if (notes.size() == 1)
    {
        if (synth->notes[note_idx] == false)
            synth->sendNoteOn(notes[note_idx], MIDI);
    }

    else if (notes.size() == 2)
    {
        for (int i = 0; i < 2; i++)
        {
            if (synth->notes[note_idx] == false)
                synth->sendNoteOn(notes[note_idx - i], MIDI);
        }
    }

    else if (notes.size() >= 3)
    {
        for (int i = 0; i < 3; i++)
        {
            if (synth->notes[note_idx] == false)
                synth->sendNoteOn(notes[note_idx - i], MIDI);
        }
    }
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
