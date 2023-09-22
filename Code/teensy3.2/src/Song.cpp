#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>

///////////////////////// SETUP FUNCTIONS /////////////////////////////
///////////////////////////////////////////////////////////////////////

bool Song::get_setup_state()
{

    bool currentState = setup_state;
    setup_state = false;
    return currentState;
}

void Song::set_notes(std::vector<int> list)
{
    // clear notes list:
    notes.clear();

    // add notes from list:
    for (uint8_t i = 0; i < list.size(); i++)
    {
        notes.push_back(list[i]);
    }

    // print list:
    Devtools::print_to_console("Score::notes:");
    for (uint8_t i = 0; i < notes.size(); i++)
    {
        Devtools::print_to_console(" ");
        Devtools::print_to_console(notes[i]);
    }
    Devtools::println_to_console("");
}

void Song::increase_step()
{
    step++;
    setup_state = true;
    Serial.println("step");
    Serial.println(step);
    Hardware::lcd->clear();
}

// proceed to step 0 of next song and set all instruments effects to "Monitor":
void Song::proceed_to_next_score() // TODO: make this a callback function/the songs' individual "tidyUp function", so they can be programmed individually.
{
    // proceed to next song in list:
    Globals::active_song_pointer = (Globals::active_song_pointer + 1) % Globals::songlist.size();
    Globals::active_song = Globals::songlist.at(Globals::active_song_pointer);
    Devtools::print_to_console("switching to song ");
    Devtools::println_to_console(Globals::active_song->name);
    // ...and begin at step 0:
    Globals::active_song->step = 0;
    Globals::active_song->setup_state = true;

    // reset effects:
    for (auto &instrument : Drumset::instruments)
        instrument->effect = Monitor;

    // turn off all notes:
    for (auto &synth : Synthesizers::synths)
    {
        for (int note = 0; note < 127; note++)
        {
            synth->notes[note] = false;
        }
    }

    Hardware::lcd->clear();
}

// set tempo of the song:
void Song::setTempoRange(int min_tempo_, int max_tempo_)
{
    Globals::active_song->tempo.min_tempo = min_tempo_;
    Globals::active_song->tempo.max_tempo = max_tempo_;
}

// reset all instruments to "Monitor" mode
void Song::resetInstruments()
{
    for (auto &drum : Drumset::instruments)
    {
        drum->set_effect(Monitor);
    }
    //TODO: also reset synthesizers' CC values and turn notes off!
}

//////////////////////////// MUSICAL FUNCTIONS ////////////////////////
void Song::add_bassNote(int note)
{
    notes.push_back(note);
    Devtools::print_to_console("note ");
    Devtools::print_to_console(note);
    Devtools::print_to_console(" has been added to Score::notes [ ");
    for (uint8_t i = 0; i < notes.size(); i++)
    {
        Devtools::print_to_console(notes[i]);
        Devtools::print_to_console(" ");
    }
    Devtools::println_to_console("]");
}

///////////////////////////////////////////////////////////////////////
////////////////////////////// MODES //////////////////////////////////
///////////////////////////////////////////////////////////////////////

// play note, repeatedly:
void Song::playRhythmicNotes(Synthesizer *synth, int rhythmic_iterator) // initiates a continuous bass note from score
{
    if (rhythmic_iterator != 0)
        note_change_pos = rhythmic_iterator;

    if ((Globals::current_beat_pos + 1) % note_change_pos == 0)
    {
        // play note
        synth->sendNoteOff(notes[note_idx]);
        synth->sendNoteOn(notes[note_idx]);
        Devtools::print_to_console("\tplaying Score::note:");
        Devtools::println_to_console(notes[note_idx]);

        // change note
        if (notes.size() > 1)
        {
            note_idx++;
            if (note_idx > int(notes.size()) - 1)
                note_idx = 0;
            //  = (note_idx + 1) % notes.size(); // iterate through the bass notes
            Devtools::print_to_console("\tnote_idx = ");
            Devtools::println_to_console(note_idx);
        }
    }
    // else

    // if (Globals::current_beat_pos == 0) // at beginninng of each bar
    // {
    //     Devtools::print_to_console("\tnotes.size() = ");
    //     Devtools::println_to_console(int(notes.size()));
    // }
}

// ----------------- play note only once (turn on never off):
// TODO: FIX THIS! if this is running, re-play note whenever it is turned off (due to too many notes..)
// void Song::playSingleNote(Synthesizer *synth, ) // initiates a continuous bass note from score
// {
//     if (notes.size() > 0)
//     {
//         if (synth->notes[note_idx] == false)
//             synth->sendNoteOn(notes[note_idx];
//     }
//     else
//         Devtools::println_to_console("cannot play MIDI note, because Score::notes is empty.");
// }

// play last 3 notes in list:
void Song::playLastThreeNotes(Synthesizer *synth)
{
    if (notes.size() == 1)
    {
        if (synth->notes[note_idx] == false)
            synth->sendNoteOn(notes[note_idx]);
    }

    else if (notes.size() == 2)
    {
        for (int i = 0; i < 2; i++)
        {
            if (synth->notes[note_idx] == false)
                synth->sendNoteOn(notes[note_idx - i]);
        }
    }

    else if (notes.size() >= 3)
    {
        for (int i = 0; i < 3; i++)
        {
            if (synth->notes[note_idx] == false)
                synth->sendNoteOn(notes[note_idx - i]);
        }
    }
}

void Song::envelope_cutoff(Synthesizer *synth, TOPOGRAPHY *topography)
{
    int cutoff_val = topography->a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height

    cutoff_val = max(20, cutoff_val);  // must be at least 20
    cutoff_val = min(cutoff_val, 127); // must not be greater than 127
    synth->sendControlChange(mKORG_Cutoff, cutoff_val);
}

void Song::envelope_volume(TOPOGRAPHY *topography, Synthesizer *synth)
{
    int amp_val = topography->a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    // amp_val = max(0, amp_val);                                    // must be at least 0
    amp_val = min(amp_val, 127); // must not be greater than 127
    synth->sendControlChange(mKORG_Amplevel, amp_val);
}

void Song::crazyDelays(Instrument *instrument, Synthesizer *synth)
{
    int delaytime = instrument->topography.a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    delaytime = min(delaytime, 127);                                               // must not be greater than 127
    synth->sendControlChange(mKORG_DelayTime, delaytime);
}

// void Score::set_ramp(, CC_Type cc_type, MIDI_Instrument midi_instr, int start_value, int end_value, int duration)
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

Song *Globals::get_song(String songName)
{
    Devtools::println_to_console(songName);
	for (auto &thisSong : songlist)
	{
        Devtools::println_to_console(thisSong->name == songName);
		if (strcmp(thisSong->name.c_str(), songName.c_str()) == 0){ //string to const char conversion
            Devtools::print_to_console("randomly selected song ");
            Devtools::println_to_console(thisSong->name);
			return thisSong;
		}
	}
    return Globals::active_song;
    Devtools::print_to_console("could not get_song ");
    Devtools::println_to_console(songName);
}