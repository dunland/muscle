#include <Globals.h>
#include <Tsunami.h>
// #include <vector>
// #include <MIDI.h>

bool TOPOGRAPHY::ready()
{
	return (average_smooth > activation_thresh);
}

boolean Globals::printStrokes = true;
boolean Globals::use_responsiveCalibration = false;
boolean Globals::do_print_to_console = true;
boolean Globals::do_send_to_processing = false;
boolean Globals::do_print_beat_sum = false; // prints Score::beat_sum topography array

IntervalTimer Globals::masterClock; // 1 bar

Tsunami Globals::tsunami;
float Globals::track_bpm[256] =
	{
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 100, 1, 1, 1, 1, 1,
		1, 1, 90, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 73, 1, 1, 1,
		100, 1, 1, 1, 200, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 41, 1, 1,
		103, 1, 1, 1, 1, 1, 1, 93, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 100, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 78, 1, 100, 100, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 100, 1, 1, 1, 1,
		1, 1, 1, 1, 100, 60};

// int swell_val[numInputs]; // this should be done in the swell section, but is needed in print section already... :/

int Globals::current_beat_pos = 0;	   // always stores the current position in the beat
int Globals::current_eighth_count = 0; // overflows at current_beat_pos % 8
int Globals::current_16th_count = 0;   // overflows at current_beat_pos % 2
int Globals::last_eighth_count = 0;	   // stores last eightNoteCount for comparison
int Globals::last_16th_count = 0;	   // stores last eightNoteCount for comparison

int Globals::tapInterval = 500; // 0.5 s per beat for 120 BPM
int Globals::current_BPM = 120;

boolean Globals::footswitch_is_pressed = false;

// ----------------------------- timer counter ---------------------------------
volatile unsigned long Globals::masterClockCount = 0; // 4*32 = 128 masterClockCount per cycle
volatile unsigned long Globals::beatCount = 0;
int Globals::next_beatCount = 0; // will be reset when timer restarts
volatile boolean Globals::sendMidiClock = false;

void Globals::masterClockTimer()
{
	/*
    timing              fraction          bar @ 120 BPM    bar @ 180 BPM
    1 bar             = 1               = 2 s       | 1.3 s
    1 beatCount       = 1/32 bar        = 62.5 ms   | 41.7 ms
    stroke precision  = 1/4 beatCount   = 15.625 ms | 10.4166 ms


    |     .-.
    |    /   \         .-.
    |   /     \       /   \       .-.     .-.     _   _
    +--/-------\-----/-----\-----/---\---/---\---/-\-/-\/\/---
    | /         \   /       \   /     '-'     '-'
    |/           '-'         '-'
    |--------2 oscil---------|
    |------snare = 10 ms-----|

  */

	masterClockCount++; // will rise infinitely

	// ------------ 1/32 increase in 4 full precisionCounts -----------
	if (masterClockCount % 4 == 0)
	{
		beatCount++; // will rise infinitely
	}

	// evaluate current position of beat in bar for stroke precision
	// 2020-09-07: this doesn't help and it's also not working...
	// if ((masterClockCount % 4) >= next_beatCount - 2)
	// {
	//   currentStep = next_beatCount;
	//   next_beatCount += 4;
	// }

	// prepare MIDI clock:
	sendMidiClock = (((masterClockCount % 4) / 3) % 4 == 0);
}
// ---------------------------------------------------------------------------

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
// void Globals::derive_topography(TOPOGRAPHY *original, TOPOGRAPHY *abstraction)
// {
// 	if (original->average_smooth < original->activation_thresh)
// 		abstraction->a_16[Globals::current_16th_count] = original->a_16[Globals::current_16th_count]; // sync both arrays

// 	else if (original->average_smooth >= original->activation_thresh) // only execute if threshold of original topography
// 	{

// 		// reset flags:
// 		abstraction->flag_empty_increased = false;
// 		abstraction->flag_empty_played = false;
// 		abstraction->flag_occupied_increased = false;
// 		abstraction->flag_occupied_missed = false;

// 		// CASES:
// 		if (original->a_16[Globals::current_16th_count] == 0 && original->a_16_prior[Globals::current_16th_count] == 0) // empty slot repeatedly not played: increase
// 		{
// 			abstraction->a_16[Globals::current_16th_count]++;
// 			abstraction->flag_empty_increased = true;
// 		}

// 		else if (original->a_16[Globals::current_16th_count] > 0 && original->a_16[Globals::current_16th_count] > original->a_16_prior[Globals::current_16th_count]) // occupied slot repeatedly played: increase
// 		{
// 			abstraction->a_16[Globals::current_16th_count]++;
// 			abstraction->flag_occupied_increased = true;
// 		}

// 		else if (original->a_16[Globals::current_16th_count] > 0 && original->a_16[Globals::current_16th_count] == original->a_16_prior[Globals::current_16th_count]) // occupied slot NOT played : decrease
// 		{
// 			abstraction->a_16[Globals::current_16th_count]--;
// 			abstraction->a_16[Globals::current_16th_count] = max(0, abstraction->a_16[Globals::current_16th_count]);
// 			abstraction->flag_occupied_missed = true;
// 		}

// 		else if (original->a_16_prior[Globals::current_16th_count] == 0 && original->a_16[Globals::current_16th_count] > 0) // empty slot PLAYED: decrease
// 		{
// 			abstraction->a_16[Globals::current_16th_count]--;
// 			abstraction->a_16[Globals::current_16th_count] = max(0, abstraction->a_16[Globals::current_16th_count]);
// 			abstraction->flag_empty_played = true;
// 		}
// 		original->a_16_prior[Globals::current_16th_count] = original->a_16[Globals::current_16th_count];
// 	}
// }

// ---------------- smoothen 16-bit array using struct ----------------
void Globals::smoothen_dataArray(TOPOGRAPHY *topography)
{
	/* input an array of size 16
1. count entries and create squared sum of each entry
2. calculate (squared) fraction of total for each entry
3. get highest of these fractions
4. get ratio of highest fraction to other and reset values if ratio > threshold
->
*/

	int len = topography->a_16.size(); // TODO: use dynamic vector topography.a instead
	int entries = 0;
	int squared_sum = 0;
	topography->regular_sum = 0;

	// count entries and create squared sum:
	for (int j = 0; j < len; j++)
	{
		if (topography->a_16[j] > 0)
		{
			entries++;
			squared_sum += topography->a_16[j] * topography->a_16[j];
			topography->regular_sum += topography->a_16[j];
		}
	}

	topography->regular_sum = topography->regular_sum / entries;

	// calculate site-specific (squared) fractions of total:
	float squared_frac[len];
	for (int j = 0; j < len; j++)
		squared_frac[j] =
			float(topography->a_16[j]) / float(squared_sum);

	// get highest frac:
	float highest_squared_frac = 0;
	for (int j = 0; j < len; j++)
		highest_squared_frac = (squared_frac[j] > highest_squared_frac) ? squared_frac[j] : highest_squared_frac;

	// SMOOTHEN ARRAY / get "topography height":
	// divide highest with other entries and reset entries if ratio > threshold:
	topography->flag_entry_dismissed = false;
	for (int j = 0; j < len; j++)
		if (squared_frac[j] > 0)
			if (highest_squared_frac / squared_frac[j] > 3 || squared_frac[j] / highest_squared_frac > topography->snr_thresh)
			{
				topography->a_16[j] = 0;
				entries -= 1;
				topography->flag_entry_dismissed = true;
			}

	topography->average_smooth = 0;
	// assess average topo sum for loudness
	for (int j = 0; j < 16; j++)
		topography->average_smooth += topography->a_16[j];
	topography->average_smooth = int((float(topography->average_smooth) / float(entries)) + 0.5);
}

// ---------------------------- DEBUG FUNCTIONS ------------------------------
void Globals::print_to_console(String message_to_print) // print String
{
	if (Globals::do_print_to_console)
		Serial.print(message_to_print);
}

void Globals::print_to_console(int int_to_print) // print int
{
	if (Globals::do_print_to_console)
		Serial.print(int_to_print);
	else if (Globals::do_send_to_processing)
		Serial.write(int_to_print);
}

void Globals::print_to_console(float float_to_print) // print float
{
	if (Globals::do_print_to_console)
		Serial.print(float_to_print);
}

void Globals::println_to_console(String message_to_print)
{
	if (Globals::do_print_to_console)
		Serial.println(message_to_print);
}

void Globals::println_to_console(int int_to_print) // print int
{
	if (Globals::do_print_to_console)
		Serial.println(int_to_print);
}

void Globals::println_to_console(float float_to_print) // print float
{
	if (Globals::do_print_to_console)
		Serial.println(float_to_print);
}

void Globals::printTopoArray(TOPOGRAPHY *topography)
{
	// print layer:
	print_to_console(topography->tag);
	print_to_console(":\t[");

	for (int j = 0; j < 16; j++)
	{
		print_to_console(topography->a_16[j]);
		if (j < 15)
			print_to_console(",");
	}

	print_to_console("] \tø");
	print_to_console(topography->average_smooth);
	print_to_console(" (");
	print_to_console(topography->average_smooth);
	print_to_console("/");
	print_to_console(topography->activation_thresh);
	print_to_console(")");

	if (topography->flag_entry_dismissed)
		print_to_console("\tentries dismissed!");
	if (topography->flag_empty_increased)
		print_to_console("\tempty slot repeatedly not played; regularity++");
	if (topography->flag_occupied_increased)
		print_to_console("\toccupied slot repeatedly played; regularity++");
	if (topography->flag_occupied_missed)
		print_to_console("\toccupied slot missed!; regularity--");
	if (topography->flag_empty_played)
		print_to_console("\tempty slot PLAYED!; regularity--");

	println_to_console("");
}

// send to processing instead:
void Globals::send_to_processing(char message_to_send) // send char
{
	if (Globals::do_send_to_processing)
		Serial.write(message_to_send);
}

void Globals::topo_array_to_processing(TOPOGRAPHY *topo)
{
	if (Globals::do_send_to_processing)
	{
		// Serial.write(topo->tag);
		for (int j = 0; j < 16; j++)
			Serial.write(topo->a_16[j]);
		Serial.write("\n");
	}
}