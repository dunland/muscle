#include <Topography.h>

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