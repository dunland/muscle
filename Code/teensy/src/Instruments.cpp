#include <Instruments.h>
#include <MIDI.h>
#include <Tsunami.h>

void Instrument::setup_notes(std::vector<int> list)
{
  for (int i = 0; i < list.size(); i++)
  {
    midi.notes.push_back(list[i]);
  }
}

void Instrument::setup_midi(CC_Type cc_type, int cc_max, int cc_min, float cc_increase_factor, float cc_decay_factor)
{
  midi.cc_chan = cc_type;
  midi.cc_max = cc_max;
  midi.cc_min = cc_min;
  midi.cc_increase_factor = cc_increase_factor;
  midi.cc_decay_factor = cc_decay_factor;
}

///////////////////////// STROKE DETECTION /////////////////////////
////////////////////////////////////////////////////////////////////
bool Instrument::stroke_detected()
{
  // static unsigned long firstPinActiveTimeCopy[Globals::numInputs];
  // static int lastValue[Globals::numInputs];    // for LED toggle
  // static boolean toggleState = false; // for LED toggle

  noInterrupts();
  timing.lastPinActiveTimeCopy = timing.lastPinActiveTime;
  // firstPinActiveTimeCopy[instr] = firstPinActiveTime[instr];
  interrupts();

  if (millis() > timing.lastPinActiveTimeCopy + sensitivity.delayAfterStroke) // get counts only X ms after LAST hit

  //if (millis() > firstPinActiveTimeCopy[instr] + globalDelayAfterStroke)
  //get counts only X ms after FIRST hit ??
  {
    static int countsCopy;
    noInterrupts();
    countsCopy = timing.counts;
    timing.counts = 0;
    interrupts();

    // ---------------------------- found significant count!
    if (countsCopy >= sensitivity.crossings)
    {
      // LED blink:
      //if (countsCopy[instr] != lastValue[instr]) toggleState = !toggleState;
      //digitalWrite(LED_BUILTIN, toggleState);
      // lastValue[instr] = countsCopy;

      // countsCopy[instr] = 0;

      return true;
    }
    else // timing ok but no significant counts
    {
      return false;
    }
  }
  else // TODO: timing not ok (obsolete: will always be ok!)
  {
    {
      return false;
    }
  }
}
// --------------------------------------------------------------------

////////////////////////////// CALCULATE NOISEFLOOR ///////////////////
///////////////////////////////////////////////////////////////////////
void Instrument::calculateNoiseFloor(Instrument *instrument)
{
  // calculates the average noise floor out of 400 samples from all inputs

  int led_idx = 0;

  Globals::print_to_console("calculating noiseFloor for ");
  Globals::print_to_console(Globals::DrumtypeToHumanreadable(instrument->drumtype));
  Globals::print_to_console(" (A");
  Globals::print_to_console(instrument->pin - 14);
  Globals::print_to_console(")");
  if (Globals::use_responsiveCalibration)
  {
    Globals::print_to_console(" ..waiting for stroke");
    while (analogRead(instrument->pin) < 700 + instrument->sensitivity.threshold)
      ; // calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
    Globals::print_to_console(" >!<");
    delay(1000); // should be long enough for drum not to oscillate anymore
  }

  int totalSamples = 0;
  boolean toggleState = false;
  for (int n = 0; n < 400; n++)
  {
    if (n % 100 == 0)
    {
      Globals::print_to_console(" . ");
      digitalWrite(instrument->led, toggleState);
      toggleState = !toggleState;
    }
    totalSamples += analogRead(instrument->pin);
  }
  instrument->sensitivity.noiseFloor = totalSamples / 400;
  digitalWrite(instrument->led, LOW);
  led_idx++;
  Globals::print_to_console("noiseFloor = ");
  Globals::println_to_console(instrument->sensitivity.noiseFloor);

  // turn LEDs off again:
    digitalWrite(instrument->led, LOW);
    instrument->output_string = "\t";
}
// --------------------------------------------------------------------

///////////////////// SET STRING FOR PLAY LOGGING /////////////////////
///////////////////////////////////////////////////////////////////////
void Instrument::setInstrumentPrintString()
{
	switch (effect)
	{
	case Monitor: // monitor: just print what is being played
		if (drumtype == Kick)
			output_string = "■\t"; // Kickdrum
		else if (drumtype == Cowbell)
			output_string = "▲\t"; // Crash
		else if (drumtype == Standtom1)
			output_string = "□\t"; // Standtom
		else if (drumtype == Standtom2)
			output_string = "O\t"; // Standtom
		else if (drumtype == Hihat)
			output_string = "x\t"; // Hi-Hat
		else if (drumtype == Tom1)
			output_string = "°\t"; // Tom 1
		else if (drumtype == Snare)
			output_string = "※\t"; // Snaredrum
		else if (drumtype == Tom2)
			output_string = "o\t"; // Tom 2
		else if (drumtype == Ride)
			output_string = "xx\t"; // Ride
		else if (drumtype == Crash1)
			output_string = "-X-\t"; // Crash
		else if (drumtype == Crash2)
			output_string = "-XX-\t"; // Crash
		break;

	case ToggleRhythmSlot: // toggle beat slot
		if (drumtype == Kick)
			output_string = "■\t"; // Kickdrum
		else if (drumtype == Cowbell)
			output_string = "▲\t"; // Crash
		else if (drumtype == Standtom1)
			output_string = "□\t"; // Standtom
		else if (drumtype == Standtom2)
			output_string = "O\t"; // Standtom
		else if (drumtype == Hihat)
			output_string = "x\t"; // Hi-Hat
		else if (drumtype == Tom1)
			output_string = "°\t"; // Tom 1
		else if (drumtype == Snare)
			output_string = "※\t"; // Snaredrum
		else if (drumtype == Tom2)
			output_string = "o\t"; // Tom 2
		else if (drumtype == Ride)
			output_string = "xx\t"; // Ride
		else if (drumtype == Crash1)
			output_string = "-X-\t"; // Crash
		else if (drumtype == Crash2)
			output_string = "-XX-\t"; // Crash
		break;

	case FootSwitchLooper: // add an ! if pinAction == 3 (replay logged rhythm)
		if (drumtype == Kick)
			output_string = "!■\t"; // Kickdrum
		else if (drumtype == Cowbell)
			output_string = "!▲\t"; // Crash
		else if (drumtype == Standtom1)
			output_string = "!□\t"; // Standtom
		else if (drumtype == Standtom2)
			output_string = "!O\t"; // Standtom
		else if (drumtype == Hihat)
			output_string = "!x\t"; // Hi-Hat
		else if (drumtype == Tom1)
			output_string = "!°\t"; // Tom 1
		else if (drumtype == Snare)
			output_string = "!※\t"; // Snaredrum
		else if (drumtype == Tom2)
			output_string = "!o\t"; // Tom 2
		else if (drumtype == Ride)
			output_string = "!xx\t"; // Ride
		else if (drumtype == Crash1)
			output_string = "!-X-\t"; // Crash
		else if (drumtype == Crash2)
			output_string = "!-XX-\t"; // Crash
		break;

		// case 5: // print swell_val for repeated MIDI notes in "swell" mode
		//   Globals::output_string[incoming_i] = swell_val[incoming_i];
		//   Globals::output_string[incoming_i] += "\t";
		//   break;
	}
}

// --------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
////////////////////////// INSTRUMENT EVENTS //////////////////////////
///////////////////////////////////////////////////////////////////////

////////////////////////////// TRIGGERS ///////////////////////////////
void Instrument::trigger(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
  switch (effect)
  {
  case PlayMidi:
    Effect::playMidi(instrument, MIDI);
    break;

  case Monitor:
    Effect::monitor(instrument);
    break;

  case ToggleRhythmSlot:
    Effect::toggleRhythmSlot(instrument);
    break;

  case FootSwitchLooper:
    Effect::footswitch_recordSlots(instrument);
    break;

  case TapTempo:
    Effect::getTapTempo();
    break;

  case Swell:
    Effect::swell_rec(instrument, MIDI);
    break;

  case TsunamiLink:
    Effect::countup_topography(instrument);
    break;

  case CymbalSwell: // swell-effect for loudness on field recordings (use on cymbals e.g.)
    // TODO: UNTESTED! (2020-10-09)
    Effect::swell_rec(instrument, MIDI);
    break;
  case TopographyLog:
    Effect::countup_topography(instrument);
    break;

  case PlayMidi_rawPin:
    Effect::playMidi_rawPin(instrument, MIDI);
    break;

  case CC_Effect_rawPin:
    Effect::cc_effect_rawPin(instrument, MIDI); // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
    break;

  default:
    break;
  }
}

///////////////////////////// TIMED EFFECTS ///////////////////////////
///////////////////////////////////////////////////////////////////////

void Instrument::perform(Instrument *instrument, std::vector<Instrument*> instruments, midi::MidiInterface<HardwareSerial> MIDI)
{
  switch (effect)
  {
  case PlayMidi:
    break;

  case Monitor:
    break;

  case ToggleRhythmSlot:
    Effect::sendMidiNotes_timed(instrument, MIDI);
    break;

  case FootSwitchLooper:
    Effect::setInstrumentSlots(instrument);
    Effect::sendMidiNotes_timed(instrument, MIDI);
    break;

  case TapTempo:
    break;

  case Swell:
    Effect::swell_perform(instrument, MIDI);
    break;

  case TsunamiLink:

    break;

  case CymbalSwell:

    break;
  case TopographyLog:
    Effect::topography_midi_effects(instrument, instruments, MIDI);
    break;

  default:
    break;
  }
}

/////////////////////////// TIDY UP FUNCTIONS /////////////////////////
///////////////////////////////////////////////////////////////////////
/* destructor for playing MIDI notes etc */

void Instrument::tidyUp(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
  switch (effect)
  {
  case PlayMidi:
    break;

  case Monitor:
    break;

  case ToggleRhythmSlot:
    break;

  case FootSwitchLooper:
    break;

  case TapTempo:
    break;

  case Swell:
    break;

  case TsunamiLink:

    break;

  case CymbalSwell:

    break;
  case TopographyLog:
    break;

  case CC_Effect_rawPin:
    Effect::decay_ccVal(instrument, MIDI); // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
    break;

  default:
    break;
  }
}

//////////////////////// SMOOTHEN TOPOGRAPHY ARRAYS ///////////////////
///////////////////////////////////////////////////////////////////////

// ---------------- smoothen 16-bit array using struct ----------------
// void Instrument::smoothen_dataArray(Instrument *instrument)
// {
//   /* input an array of size 16
// 1. count entries and create squared sum of each entry
// 2. calculate (squared) fraction of total for each entry
// 3. get highest of these fractions
// 4. get ratio of highest fraction to other and reset values if ratio > threshold
// ->
// */

//   // int len = *(&instrument->topography.a_16 + 1) - instrument->topography.a_16;
//   int len = instrument->topography.a_16.size(); // TODO: use dynamic vector topography.a instead
//   int entries = 0;
//   int squared_sum = 0;
//   instrument->topography.regular_sum = 0;

//   // count entries and create squared sum:
//   for (int j = 0; j < len; j++)
//   {
//     if (instrument->topography.a_16[j] > 0)
//     {
//       entries++;
//       squared_sum += instrument->topography.a_16[j] * instrument->topography.a_16[j];
//       instrument->topography.regular_sum += instrument->topography.a_16[j];
//     }
//   }

//   instrument->topography.regular_sum = instrument->topography.regular_sum / entries;

//   // calculate site-specific (squared) fractions of total:
//   float squared_frac[len];
//   for (int j = 0; j < len; j++)
//     squared_frac[j] =
//         float(instrument->topography.a_16[j]) / float(squared_sum);

//   // get highest frac:
//   float highest_squared_frac = 0;
//   for (int j = 0; j < len; j++)
//     highest_squared_frac = (squared_frac[j] > highest_squared_frac) ? squared_frac[j] : highest_squared_frac;

//   // get "topography height":
//   // divide highest with other entries and reset entries if ratio > threshold:
//   for (int j = 0; j < len; j++)
//     if (squared_frac[j] > 0)
//       if (highest_squared_frac / squared_frac[j] > 3 || squared_frac[j] / highest_squared_frac > instrument->topography.snr_thresh)
//       {
//         instrument->topography.a_16[j] = 0;
//         entries -= 1;
//       }

//   instrument->topography.average_smooth = 0;
//   // assess average topo sum for loudness
//   for (int j = 0; j < 8; j++)
//     instrument->topography.average_smooth += instrument->topography.a_16[j];
//   instrument->topography.average_smooth = int((float(instrument->topography.average_smooth) / float(entries)) + 0.5);
// }