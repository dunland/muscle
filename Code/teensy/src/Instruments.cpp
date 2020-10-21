#include <Instruments.h>
#include <MIDI.h>
#include <Tsunami.h>

<<<<<<< HEAD

///////////////////////// STROKE DETECTION /////////////////////////
////////////////////////////////////////////////////////////////////
bool Instrument::stroke_detected(Instrument* instrument)
=======
void Instrument::setup_notes(std::vector<int> list)
{
  for (int i = 0; i < list.size(); i++)
  {
    midi.notes.push_back(list[i]);
  }
}

///////////////////////// STROKE DETECTION /////////////////////////
////////////////////////////////////////////////////////////////////
bool Instrument::stroke_detected(Instrument *instrument)
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
{
  static unsigned long lastPinActiveTimeCopy[Globals::numInputs];
  // static unsigned long firstPinActiveTimeCopy[Globals::numInputs];
  // static int lastValue[Globals::numInputs];    // for LED toggle
  // static boolean toggleState = false; // for LED toggle

  noInterrupts();
  lastPinActiveTimeCopy[instrument->drumtype] = instrument->timing.lastPinActiveTime;
  // firstPinActiveTimeCopy[instr] = firstPinActiveTime[instr];
  interrupts();

  if (millis() > lastPinActiveTimeCopy[instrument->drumtype] + instrument->sensitivity.delayAfterStroke) // get counts only X ms after LAST hit

  //if (millis() > firstPinActiveTimeCopy[instr] + globalDelayAfterStroke)
  //get counts only X ms after FIRST hit ??
  {
    static int countsCopy;
    noInterrupts();
    countsCopy = instrument->timing.counts;
    instrument->timing.counts = 0;
    interrupts();

    // ---------------------------- found significant count!
    if (countsCopy >= instrument->sensitivity.crossings)
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
<<<<<<< HEAD
void Instrument::calculateNoiseFloor(Instrument* instrument)
=======
void Instrument::calculateNoiseFloor(Instrument *instrument)
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
{
  // calculates the average noise floor out of 400 samples from all inputs

  int led_idx = 0;
<<<<<<< HEAD
  
    Serial.print("calculating noiseFloor for ");
    Serial.print(Globals::DrumtypeToHumanreadable(instrument->drumtype));
    Serial.print(" ..waiting for stroke");
    if (Globals::use_responsiveCalibration)
    {
      while (analogRead(instrument->pin) < 700 + instrument->sensitivity.threshold)
        ; // calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
      Serial.print(" .");
      delay(1000); // should be long enough for drum not to oscillate anymore
    }

    int totalSamples = 0;
    boolean toggleState = false;
    for (int n = 0; n < 400; n++)
    {
      if (n % 100 == 0)
      {
        Serial.print(" . ");
        digitalWrite(Globals::leds[instrument->drumtype], toggleState);
        toggleState = !toggleState;
      }
      totalSamples += analogRead(instrument->pin);
    }
    instrument->sensitivity.noiseFloor = totalSamples / 400;
    digitalWrite(Globals::leds[instrument->drumtype], LOW);
    led_idx++;
    Serial.print("noiseFloor = ");
    Serial.println(instrument->sensitivity.noiseFloor);
  
=======

  Serial.print("calculating noiseFloor for ");
  Serial.print(Globals::DrumtypeToHumanreadable(instrument->drumtype));
  Serial.print(" ..waiting for stroke");
  if (Globals::use_responsiveCalibration)
  {
    while (analogRead(instrument->pin) < 700 + instrument->sensitivity.threshold)
      ; // calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
    Serial.print(" .");
    delay(1000); // should be long enough for drum not to oscillate anymore
  }

  int totalSamples = 0;
  boolean toggleState = false;
  for (int n = 0; n < 400; n++)
  {
    if (n % 100 == 0)
    {
      Serial.print(" . ");
      digitalWrite(Globals::leds[instrument->drumtype], toggleState);
      toggleState = !toggleState;
    }
    totalSamples += analogRead(instrument->pin);
  }
  instrument->sensitivity.noiseFloor = totalSamples / 400;
  digitalWrite(Globals::leds[instrument->drumtype], LOW);
  led_idx++;
  Serial.print("noiseFloor = ");
  Serial.println(instrument->sensitivity.noiseFloor);
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e

  for (int i = 0; i < Globals::numInputs; i++) // turn LEDs off again
  {
    digitalWrite(Globals::leds[i], LOW);
    Globals::output_string[i] = "\t";
  }
}
// --------------------------------------------------------------------

<<<<<<< HEAD
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

        break;

    case CymbalSwell: // swell-effect for loudness on field recordings (use on cymbals e.g.)
        // TODO: UNTESTED! (2020-10-09)
        Effect::swell_rec(instrument, MIDI);
        break;
    case TopographyLog:

        break;

    default:
        break;
    }
=======

///////////////////////////////////////////////////////////////////////
////////////////////////// INSTRUMENT EVENTS //////////////////////////
///////////////////////////////////////////////////////////////////////

////////////////////////////// TRIGGERS ///////////////////////////////
///////////////////////////////////////////////////////////////////////
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

  default:
    break;
  }
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
}

///////////////////////////// TIMED EFFECTS ///////////////////////////
///////////////////////////////////////////////////////////////////////

<<<<<<< HEAD
void Instrument::perform(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
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
        Effect::topography_midi_effects(instrument, MIDI);
        break;

    default:
        break;
    }
=======
void Instrument::perform(Instrument *instrument, Instrument *instruments[Globals::numInputs], midi::MidiInterface<HardwareSerial> MIDI)
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
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
}

/////////////////////////// TIDY UP FUNCTIONS /////////////////////////
///////////////////////////////////////////////////////////////////////
/* destructor for playing MIDI notes etc */

void Instrument::tidyUp(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
<<<<<<< HEAD
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

    default:
        break;
    }
}


=======
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

  default:
    break;
  }
}

>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
//////////////////////// SMOOTHEN TOPOGRAPHY ARRAYS ///////////////////
///////////////////////////////////////////////////////////////////////

// ---------------- smoothen 16-bit array using struct ----------------
<<<<<<< HEAD
void Instrument::smoothen_dataArray(Instrument* instrument)
=======
void Instrument::smoothen_dataArray(Instrument *instrument)
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
{
  /* input an array of size 16
1. count entries and create squared sum of each entry
2. calculate (squared) fraction of total for each entry
3. get highest of these fractions
4. get ratio of highest fraction to other and reset values if ratio > threshold
->  
*/

<<<<<<< HEAD
  int len = *(&instrument->topography.a_16 + 1) - instrument->topography.a_16;
=======
  // int len = *(&instrument->topography.a_16 + 1) - instrument->topography.a_16;
  int len = instrument->topography.a_16.size(); // TODO: use dynamic vector topography.a instead
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
  int entries = 0;
  int squared_sum = 0;
  instrument->topography.regular_sum = 0;

  // count entries and create squared sum:
  for (int j = 0; j < len; j++)
  {
    if (instrument->topography.a_16[j] > 0)
    {
      entries++;
      squared_sum += instrument->topography.a_16[j] * instrument->topography.a_16[j];
      instrument->topography.regular_sum += instrument->topography.a_16[j];
    }
  }

  instrument->topography.regular_sum = instrument->topography.regular_sum / entries;

  // calculate site-specific (squared) fractions of total:
  float squared_frac[len];
  for (int j = 0; j < len; j++)
    squared_frac[j] =
        float(instrument->topography.a_16[j]) / float(squared_sum);

  // get highest frac:
  float highest_squared_frac = 0;
  for (int j = 0; j < len; j++)
    highest_squared_frac = (squared_frac[j] > highest_squared_frac) ? squared_frac[j] : highest_squared_frac;

  // get "topography height":
  // divide highest with other entries and reset entries if ratio > threshold:
  for (int j = 0; j < len; j++)
    if (squared_frac[j] > 0)
      if (highest_squared_frac / squared_frac[j] > 3 || squared_frac[j] / highest_squared_frac > instrument->topography.threshold)
      {
        instrument->topography.a_16[j] = 0;
        entries -= 1;
      }

  instrument->topography.average_smooth = 0;
  // assess average topo sum for loudness
  for (int j = 0; j < 8; j++)
    instrument->topography.average_smooth += instrument->topography.a_16[j];
  instrument->topography.average_smooth = int((float(instrument->topography.average_smooth) / float(entries)) + 0.5);
}