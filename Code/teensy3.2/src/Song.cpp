#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

bool TOPOGRAPHY::ready()
{
	return (average_smooth > activation_thresh);
}

// ----------------------------- TOPOGRAPHIES -------------------------
void TOPOGRAPHY::reset()
{
	a_16 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

void TOPOGRAPHY::add(TOPOGRAPHY *to_add)
{
	// sum up all topographies of all instruments:
	for (int idx = 0; idx < 16; idx++) // each slot
	{
		a_16[idx] += to_add->a_16[idx];
	}
}

// TODO:
//makeTopo();
// works like this:
// instruments[0]->smoothen_dataArray(instruments[0]);
// LOTS OF PSEUDOCODE HERE:

//    beat_topography_16.smoothen(); // → gives you beat_topography.average_smoothened_height
//
//    if (beat_topography_16.average_smoothened_height >= beat_topography.threshold)
//    {
//      // create next abstraction layer
//      int[] beat_regularity = new int[16];
//
//      // update abstraction layer:
//      for (int i = 0; i < 16; i++)
//        beat_regularity[i] = beat_topography_16[i];
//
//      // get average height again:
//      beat_regularity.smoothen();
//    }
//
//    if (beat_regularity.amplitude >= beat_regularity.threshold)
//    {
//      // you are playing super regularly. that's awesome. now activate next element in score
//      score_next_element_ready = true;
//    }
//
//    // if regularity is held for certain time (threshold reached):
//    read_to_break
//
//    when THEN regularity expectations not fulfilled:
//    score: go to next element
//
//    // elsewhere:
//    if (hit_certain_trigger)
//    {
//
//      if (score_next_element_ready)
//      {
//        /* do whatever is up next in the score */
//      }
//    }

/* Score could look like this:
      intro----------part 1--...-----outro--------
      1     2     3     4    ...     20    21     step
      ++    ++    ++                              element_FX
                  ++    ++   ...     ++    ++     element_notes
                        ++           ++           element_fieldRecordings

      cool thing: create score dynamically according to how I play
    */

// PSEUDOCODE END
// -->
void TOPOGRAPHY::derive_from(TOPOGRAPHY *original)
{
	if (original->average_smooth < original->activation_thresh)
		a_16[Globals::current_16th_count] = original->a_16[Globals::current_16th_count]; // sync both arrays

	else if (original->average_smooth >= original->activation_thresh) // only execute if threshold of original topography
	{

		// always check for previous beat position:
		int beat_position;
		if (Globals::current_16th_count > 0)
			beat_position = Globals::current_16th_count - 1;
		else
			beat_position = 15;

		// reset flags (for console output)
		flag_empty_increased[beat_position] = false;
		flag_empty_played[beat_position] = false;
		flag_occupied_increased[beat_position] = false;
		flag_occupied_missed[beat_position] = false;

		// CASES:

		// empty slot repeatedly not played: increase:
		if (original->a_16[beat_position] == 0 && a_16[beat_position] == 0)
		{
			regularity++;
			flag_empty_increased[beat_position] = true;
		}

		// occupied slot repeatedly played: increase
		else if (original->a_16[beat_position] > 0 && original->a_16[beat_position] > a_16[beat_position])
		{
			regularity++;
			flag_occupied_increased[beat_position] = true;
		}

		// occupied slot NOT played : decrease
		else if (original->a_16[beat_position] > 0 && original->a_16[beat_position] <= a_16[beat_position])
		{
			regularity--;
			regularity = max(0, regularity);
			flag_occupied_missed[beat_position] = true;
		}

		// empty slot PLAYED: decrease
		else if (a_16[beat_position] == 0 && original->a_16[beat_position] > 0)
		{
			regularity--;
			regularity = max(0, regularity);
			flag_empty_played[beat_position] = true;
		}

		// copy contents:
		a_16[beat_position] = original->a_16[beat_position];
	}
}

// ---------------- smoothen 16-bit array using struct ----------------
void TOPOGRAPHY::smoothen_dataArray()
{
	/* input an array of size 16
1. count entries and create squared sum of each entry
2. calculate (squared) fraction of total for each entry
3. get highest of these fractions
4. get ratio of highest fraction to other and reset values if ratio > threshold
->
*/

	int len = a_16.size(); // TODO: use dynamic vector topography.a instead
	int entries = 0;
	int squared_sum = 0;
	regular_sum = 0;

	// count entries and create squared sum:
	for (int j = 0; j < len; j++)
	{
		if (a_16[j] > 0)
		{
			entries++;
			squared_sum += a_16[j] * a_16[j];
			regular_sum += a_16[j];
		}
	}

	regular_sum = regular_sum / entries;

	// calculate site-specific (squared) fractions of total:
	float squared_frac[len];
	for (int j = 0; j < len; j++)
		squared_frac[j] =
			float(a_16[j]) / float(squared_sum);

	// get highest frac:
	float highest_squared_frac = 0;
	for (int j = 0; j < len; j++)
		highest_squared_frac = (squared_frac[j] > highest_squared_frac) ? squared_frac[j] : highest_squared_frac;

	// SMOOTHEN ARRAY / get "topography height":
	// divide highest with other entries and reset entries if ratio > threshold:
	flag_entry_dismissed = false;
	for (int j = 0; j < len; j++)
		if (squared_frac[j] > 0)
			if (highest_squared_frac / squared_frac[j] > 3 || squared_frac[j] / highest_squared_frac > snr_thresh)
			{
				a_16[j] = 0;
				entries -= 1;
				flag_entry_dismissed = true;
			}

	average_smooth = 0;
	// assess average topo sum for loudness
	for (int j = 0; j < 16; j++)
		average_smooth += a_16[j];
	average_smooth = int((float(average_smooth) / float(entries)) + 0.5);
}

void TOPOGRAPHY::print()
{
	// print layer:
	Devtools::print_to_console(tag);
	Devtools::print_to_console(":\t[");

	for (int j = 0; j < 16; j++)
	{
		Devtools::print_to_console(a_16[j]);
		if (j < 15)
			Devtools::print_to_console(",");
	}

	Devtools::print_to_console("] \tø");
	Devtools::print_to_console(average_smooth);
	Devtools::print_to_console(" (");
	Devtools::print_to_console(average_smooth);
	Devtools::print_to_console("/");
	Devtools::print_to_console(activation_thresh);
	Devtools::print_to_console(")");

	if (flag_entry_dismissed)
		Devtools::print_to_console("\tentries dismissed!");
	if (tag == "r")
	{
		Devtools::print_to_console("regularity = ");
		Devtools::print_to_console(regularity);
	}
	if (flag_empty_increased)
		Devtools::print_to_console("\tempty slot repeatedly not played; regularity++");
	if (flag_occupied_increased)
		Devtools::print_to_console("\toccupied slot repeatedly played; regularity++");
	if (flag_occupied_missed)
		Devtools::print_to_console("\toccupied slot missed!; regularity--");
	if (flag_empty_played)
		Devtools::print_to_console("\tempty slot PLAYED!; regularity--");

	Devtools::println_to_console("");
}

////////////////////////// SETUP FUNCTIONS ////////////////////////////
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

void Song::set_step_function(int trigger_step, Instrument *instrument, EffectsType effect_)
{
}

void Song::increase_step()
{
    step++;
    setup_state = true;
    Serial.println("step");
    Serial.println(step);
    Hardware::lcd->clear();
}

// proceed to step 0 of next score and set all instruments effects to "Monitor":
void Song::proceed_to_next_score()
{
    // proceed to next score in list:
    Globals::active_score_pointer = (Globals::active_score_pointer + 1) % Globals::songlist.size();
    Globals::active_song = Globals::songlist[Globals::active_score_pointer];
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

// set tempo of the score:
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
void Song::playRhythmicNotes(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI, int rhythmic_iterator) // initiates a continuous bass note from score
{
    if (rhythmic_iterator != 0)
        note_change_pos = rhythmic_iterator;

    if ((Globals::current_beat_pos + 1) % note_change_pos == 0)
    {
        // play note
        synth->sendNoteOff(notes[note_idx], MIDI);
        synth->sendNoteOn(notes[note_idx], MIDI);
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
// TODO: if this is running, re-play note whenever it is turned off (due to too many notes..)
void Song::playSingleNote(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI) // initiates a continuous bass note from score
{
    if (notes.size() > 0)
    {
        if (synth->notes[note_idx] == false)
            synth->sendNoteOn(notes[note_idx], MIDI);
    }
    else
        Devtools::println_to_console("cannot play MIDI note, because Score::notes is empty.");
}

// play last 3 notes in list:
void Song::playLastThreeNotes(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI)
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

void Song::envelope_cutoff(Synthesizer *synth, TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI)
{
    int cutoff_val = topography->a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height

    cutoff_val = max(20, cutoff_val);  // must be at least 20
    cutoff_val = min(cutoff_val, 127); // must not be greater than 127
    synth->sendControlChange(mKORG_Cutoff, cutoff_val, MIDI);
}

void Song::envelope_volume(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth)
{
    int amp_val = topography->a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    // amp_val = max(0, amp_val);                                    // must be at least 0
    amp_val = min(amp_val, 127); // must not be greater than 127
    synth->sendControlChange(mKORG_Amplevel, amp_val, MIDI);
}

void Song::crazyDelays(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth)
{
    int delaytime = instrument->topography.a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    delaytime = min(delaytime, 127);                                               // must not be greater than 127
    synth->sendControlChange(mKORG_DelayTime, delaytime, MIDI);
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

