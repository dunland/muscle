/*
   ------------------------------------
   October 2020
   by David Unland david[at]unland[dot]eu
   ------------------------------------
   ------------------------------------
  0. calibrate sensors
  1. set calibration[instrument][0/1] values. (in setup())
    0:threshold, 1:min num of threshold crossings
  2. set mode for instruments:

    pinActions:
    -----------
    0 = play MIDI note upon stroke
    1 = binary beat logger (print beat)
    2 = toggle rhythm_slot
    3 = footswitch looper: records what is being played for one bar while footswitch is pressed and repeats it after release.
    4 = tapTempo: a standard tapTempo to change the overall pace. to be used on one instrument only.
    5 = "swell" effect: all instruments have a tap tempo that will change MIDI CC values when played a lot (values decrease automatically)
    6 = Tsunami beat-linked playback: finds patterns within 1 bar for each instrument and plays an according rhythmic sample from tsunami database
    7 = using swell effect for tsunami playback loudness (arhythmic field recordings for cymbals)
    8 = 16th-note-topography with MIDI playback and volume change
*/

/* --------------------------------------------------------------------- */
/* ------------------------------- GLOBAL ------------------------------ */
/* --------------------------------------------------------------------- */
#include <Arduino.h>
#include <vector>
#include <MIDI.h>
#include <Tsunami.h>
#include <Globals.h>
#include <Instruments.h>
#include <Effects.h>

midi::MidiInterface<HardwareSerial> MIDI((HardwareSerial &)Serial2); // same as // MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

Instrument *instruments[Globals::numInputs];

// ------------------------- Debug variables --------------------------
boolean use_responsiveCalibration = false;
boolean printNormalizedValues_ = false;
boolean do_print_to_console = true;
boolean do_send_to_processing = false;

// ------------------------- interrupt timers -------------------------
IntervalTimer pinMonitor; // reads pins every 1 ms

// -------------------- timing and rhythm tracking --------------------
int countsCopy[Globals::numInputs];

// ----------------- MUSICAL AND PERFORMATIVE PARAMETERS --------------

const int LOG_BEATS = 0;
const int HOLD_CC = 1;
const int RESET_TOPO = 2; // resets beat_topography (of all instruments)
const int FOOTSWITCH_MODE = 2;

/*
    0 = play MIDI note upon stroke
    1 = binary beat logger (print beat)
    2 = toggle rhythm_slot
    3 = footswitch looper: records what is being played for one bar while footswitch is pressed and repeats it after release.
    4 = tapTempo: a standard tapTempo to change the overall pace. to be used on one instrument only.
    5 = "swell" effect: all instruments have a tap tempo that will change MIDI CC values when played a lot (values decrease automatically)
    6 = Tsunami beat-linked playback: finds patterns within 1 bar for each instrument and plays an according rhythmic sample from tsunami database
    7 = using swell effect for tsunami playback loudness (arhythmic field recordings for cymbals)
    8 = 16th-note-topography with MIDI playback and volume changet
*/
// int initialPinAction[Globals::numInputs];
// int allocated_track[Globals::numInputs];                   // tracks will be allocated in tsunami_beat_playback
// int allocated_channels[] = {0, 0, 0, 0, 0, 0, 0}; // channels to send audio from tsunami to

struct TOPOGRAPHY_16 // must be global!
{
  int topo_array[16];
  int threshold;
  int average_smooth;
} beat_topography_8_, beat_topography_16_, beat_regularity_16_;

/////////////////////////// general pin reading ///////////////////////
///////////////////////////////////////////////////////////////////////

int pinValue(int instr)
{
  return abs(instruments[instr]->sensitivity.noiseFloor - analogRead(instruments[instr]->pin));
}
// --------------------------------------------------------------------

// /* --------------------------------------------------------------------- */
// /* -------------------------- TIMED INTERRUPTS ------------------------- */
// /* --------------------------------------------------------------------- */
void samplePins()
{
  // ------------------------- read all pins -----------------------------------
  for (int pinNum = 0; pinNum < Globals::numInputs; pinNum++)
  {
    if (pinValue(pinNum) > instruments[pinNum]->sensitivity.threshold)
    {
      if (instruments[pinNum]->timing.counts < 1)
        instruments[pinNum]->timing.firstPinActiveTime = millis();
      instruments[pinNum]->timing.lastPinActiveTime = millis();
      instruments[pinNum]->timing.counts++;
    }
  }
}

// -----------------------------------------------------------------------------

/* --------------------------------------------------------------------- */
/* ----------------------------- FUNCTIONS ----------------------------- */
/* --------------------------------------------------------------------- */

////////////////////////////// CALCULATE NOISEFLOOR ///////////////////
///////////////////////////////////////////////////////////////////////
void calculateNoiseFloor()
{
  // calculates the average noise floor out of 400 samples from all inputs

  int led_idx = 0;
  for (int pinNum = 0; pinNum < Globals::numInputs; pinNum++)
  {
    Serial.print("calculating noiseFloor for ");
    Serial.print(Globals::DrumtypeToHumanreadable(DrumType(pinNum)));
    Serial.print(" ..waiting for stroke");
    if (use_responsiveCalibration)
    {
      while (analogRead(instruments[pinNum]->pin) < 700 + instruments[pinNum]->sensitivity.threshold)
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
        digitalWrite(Globals::leds[pinNum], toggleState);
        toggleState = !toggleState;
      }
      totalSamples += analogRead(instruments[pinNum]->pin);
    }
    instruments[pinNum]->sensitivity.noiseFloor = totalSamples / 400;
    digitalWrite(Globals::leds[pinNum], LOW);
    led_idx++;
    Serial.print("noiseFloor = ");
    Serial.println(instruments[pinNum]->sensitivity.noiseFloor);
  }

  for (int i = 0; i < Globals::numInputs; i++) // turn LEDs off again
  {
    digitalWrite(Globals::leds[i], LOW);
    Globals::output_string[i] = "\t";
  }
}
// --------------------------------------------------------------------

////////////////////////////////// FOOT SWITCH ////////////////////////
///////////////////////////////////////////////////////////////////////
EffectsType lastEffect[Globals::numInputs];

// int swell_val[Globals::numInputs];
// boolean footswitch_is_pressed = false;

void footswitch_pressed()
{
  switch (FOOTSWITCH_MODE)
  {
  case (LOG_BEATS):
    // set pinMode of all instruments to 3 (record what is being played)
    for (int i = 0; i < Globals::numInputs; i++)
    {
      lastEffect[i] = instruments[i]->effect;    // TODO: lastPinAction seems to be overwritten quickly, so it will not be able to return to its former state. fix this!
      instruments[i]->effect = FootSwitchLooper; // TODO: not for Cowbell?
      for (int j = 0; j < 8; j++)
        instruments[i]->score.set_rhythm_slot[j] = false; // reset entire record
    }
    break;

  case (HOLD_CC): // prevents swell_vals to be changed in swell_beat()
    Globals::footswitch_is_pressed = true;
    break;

  case (RESET_TOPO): // resets beat_topography (for all instruments)
    for (int i = 0; i < Globals::numInputs; i++)
    {
      // reset 8th-note-topography:
      for (int j = 0; j < 8; j++)
      {
        instruments[i]->topography.a_8[j] = 0;
      }

      // reset 16th-note-topography:
      for (int j = 0; j < 16; j++)
      {
        instruments[i]->topography.a_16[j] = 0;
      }
    }

    break;

  default:
    Serial.println("Footswitch mode not defined!");
    break;
  }
}
// --------------------------------------------------------------------

void footswitch_released()
{
  switch (FOOTSWITCH_MODE)
  {
  case (LOG_BEATS):
    for (int i = 0; i < Globals::numInputs; i++)
      instruments[i]->effect = instruments[i]->initialEffect;
    break;

  case (HOLD_CC):
    Globals::footswitch_is_pressed = false;
    break;

  default:
    break;
  }
}

void checkFootSwitch()
{

  static int switch_state;
  static int last_switch_state = HIGH;
  static unsigned long last_switch_toggle = 1000; // some pre-delay to prevent initial misdetection
  //static boolean lastPinAction[Globals::numInputs];

  switch_state = digitalRead(FOOTSWITCH);
  if (switch_state != last_switch_state && millis() > last_switch_toggle + 20)
  {
    if (switch_state == LOW)
    {
      footswitch_pressed();
      Serial.println("Footswitch pressed.");
    }
    else
    {
      footswitch_released();
      Serial.println("Footswitch released.");
    }
    last_switch_state = switch_state;
    last_switch_toggle = millis();
  }
}
// --------------------------------------------------------------------

// ///////////////////////// STROKE DETECTION /////////////////////////
// ////////////////////////////////////////////////////////////////////
boolean stroke_detected(int instr)
{
  static unsigned long lastPinActiveTimeCopy[Globals::numInputs];
  // static unsigned long firstPinActiveTimeCopy[Globals::numInputs];
  // static int lastValue[Globals::numInputs];    // for LED toggle
  // static boolean toggleState = false; // for LED toggle

  noInterrupts();
  lastPinActiveTimeCopy[instr] = instruments[instr]->timing.lastPinActiveTime;
  // firstPinActiveTimeCopy[instr] = firstPinActiveTime[instr];
  interrupts();

  if (millis() > lastPinActiveTimeCopy[instr] + instruments[instr]->sensitivity.delayAfterStroke) // get counts only X ms after LAST hit

  //if (millis() > firstPinActiveTimeCopy[instr] + globalDelayAfterStroke)
  //get counts only X ms after FIRST hit ??
  {
    static int countsCopy;
    noInterrupts();
    countsCopy = instruments[instr]->timing.counts;
    instruments[instr]->timing.counts = 0;
    interrupts();

    // ---------------------------- found significant count!
    if (countsCopy >= instruments[instr]->sensitivity.crossings)
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


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
/////////////////////////// DEBUG FUNCTIONS ///////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

// print the play log to Arduino console:
void print_to_console(String message_to_print)
{
  if (do_print_to_console)
    Serial.print(message_to_print);
}

void println_to_console(String message_to_print)
{
  if (do_print_to_console)
    Serial.println(message_to_print);
}

// or send stuff to processing:
void send_to_processing(int message_to_send)
{
  if (do_send_to_processing)
    Serial.write(message_to_send);
}

// --------------------------------------------------------------------

//////////////////////////// PRINT NORMALIZED VALUES //////////////////
///////////////////////////////////////////////////////////////////////
void printNormalizedValues(boolean printNorm_criterion)
{
  // useful debugger for column-wise output of raw/normalised values:

  if (printNorm_criterion == true)
  {
    static unsigned long lastMillis;
    if (millis() != lastMillis)
    {
      for (int i = 0; i < Globals::numInputs; i++)
      {
        // static int countsCopy[Globals::numInputs];
        //noInterrupts();
        //countsCopy[i] = counts[i];
        //interrupts();
        //Serial.print(pins[i]);
        //Serial.print(":\t");
        Serial.print(pinValue(i));
        Serial.print("\t");
        //Serial.print(", ");
        //Serial.print(countsCopy[i]);
      }
      Serial.println("");
    }
    lastMillis = millis();
  }
}
// --------------------------------------------------------------------

//////////////////////// SMOOTHEN TOPOGRAPHY ARRAYS ///////////////////
///////////////////////////////////////////////////////////////////////

// ---------------- smoothen 16-bit array using struct ----------------
void smoothen_dataArray(struct TOPOGRAPHY_16 *struct_ptr)
{
  int *input_array = (*struct_ptr).topo_array;
  int entries = 0;
  int squared_sum = 0;
  int regular_sum = 0;

  // count entries and create squared sum:
  for (int j = 0; j < 16; j++)
  {
    if (input_array[j] > 0)
    {
      entries++;
      squared_sum += input_array[j] * input_array[j];
      regular_sum += input_array[j];
    }
  }

  regular_sum = regular_sum / entries;

  // calculate site-specific (squared) fractions of total:
  float squared_frac[16];
  for (int j = 0; j < 16; j++)
    squared_frac[j] =
        float(input_array[j]) / float(squared_sum);

  // get highest frac:
  float highest_squared_frac = 0;
  for (int j = 0; j < 16; j++)
    highest_squared_frac = (squared_frac[j] > highest_squared_frac) ? squared_frac[j] : highest_squared_frac;

  // get "topography height":
  // divide highest with other entries and reset entries if ratio > 3:
  for (int j = 0; j < 16; j++)
    if (squared_frac[j] > 0)
      if (highest_squared_frac / squared_frac[j] > 3 || squared_frac[j] / highest_squared_frac > (*struct_ptr).threshold)
      {
        input_array[j] = 0;
        entries -= 1;
      }

  (*struct_ptr).average_smooth = 0;
  // assess average topo sum for loudness
  for (int j = 0; j < 8; j++)
    (*struct_ptr).average_smooth += input_array[j];
  struct_ptr->average_smooth = int((float((*struct_ptr).average_smooth) / float(entries)) + 0.5);
}

// --------------- smoothen 8-bit array holding instrument ------------
void smoothen_dataArray(int input_array[Globals::numInputs][8], int instr_in, int threshold_to_omit_entry)
{
  int entries = 0;
  int squared_sum = 0;
  int regular_sum = 0;

  // count entries and create squared sum:
  for (int j = 0; j < 8; j++)
  {
    if (input_array[instr_in][j] > 0)
    {
      entries++;
      squared_sum += input_array[instr_in][j] * input_array[instr_in][j];
      regular_sum += input_array[instr_in][j];
    }
  }

  regular_sum = regular_sum / entries;

  // calculate site-specific (squared) fractions of total:
  float squared_frac[8];
  for (int j = 0; j < 8; j++)
    squared_frac[j] =
        float(input_array[instr_in][j]) / float(squared_sum);

  // get highest frac:
  float highest_squared_frac = 0;
  for (int j = 0; j < 8; j++)
    highest_squared_frac = (squared_frac[j] > highest_squared_frac) ? squared_frac[j] : highest_squared_frac;

  // get "topography height":
  // divide highest with other entries and reset entries if ratio > 3:
  for (int j = 0; j < 8; j++)
    if (squared_frac[j] > 0)
      if (highest_squared_frac / squared_frac[j] > 3 || squared_frac[j] / highest_squared_frac > threshold_to_omit_entry)
      {
        input_array[instr_in][j] = 0;
        entries -= 1;
      }
}

// ------------------------ smoothen 16-bit array ---------------------
void smoothen_dataArray(int input_array[16], int threshold_to_omit_entry)
{
  int entries = 0;
  int squared_sum = 0;
  int regular_sum = 0;

  // count entries and create squared sum:
  for (int j = 0; j < 16; j++)
  {
    if (input_array[j] > 0)
    {
      entries++;
      squared_sum += input_array[j] * input_array[j];
      regular_sum += input_array[j];
    }
  }

  regular_sum = regular_sum / entries;

  // calculate site-specific (squared) fractions of total:
  float squared_frac[16];
  for (int j = 0; j < 16; j++)
    squared_frac[j] =
        float(input_array[j]) / float(squared_sum);

  // get highest frac:
  float highest_squared_frac = 0;
  for (int j = 0; j < 16; j++)
    highest_squared_frac = (squared_frac[j] > highest_squared_frac) ? squared_frac[j] : highest_squared_frac;

  // get "topography height":
  // divide highest with other entries and reset entries if ratio > 3:
  for (int j = 0; j < 16; j++)
    if (squared_frac[j] > 0)
      if (highest_squared_frac / squared_frac[j] > 3 || squared_frac[j] / highest_squared_frac > threshold_to_omit_entry)
      {
        input_array[j] = 0;
        entries -= 1;
      }
}

/* --------------------------------------------------------------------- */
/* ---------------------------------- SETUP ---------------------------- */
/* --------------------------------------------------------------------- */

void setup()
{

  // instantiate instruments:
  for (int i = 0; i < Globals::numInputs; i++)
  {
    instruments[i] = new Instrument;
  }

  Serial.begin(115200);
  // Serial3.begin(57600); // contained in tsunami.begin()
  while (!Serial)
    ; // prevents Serial flow from just stopping at some (early) point.
  // delay(1000); // alternative to line above, if run with external power (no computer)

  MIDI.begin(MIDI_CHANNEL_OMNI);

  delay(1000);     // wait for Tsunami to finish reset // redundant?
  Globals::tsunami.start(); // Tsunami startup at 57600. ATTENTION: Serial Channel is selected in Tsunami.h !!!
  delay(100);
  Globals::tsunami.stopAllTracks(); // in case Tsunami was already playing.
  Globals::tsunami.samplerateOffset(0, 0);
  Globals::tsunami.setReporting(true); // Enable track reporting from the Tsunami
  delay(100);                 // some time for Tsunami to respond with version string

  //------------------------ initialize pins and arrays ------------------------
  for (int i = 0; i < Globals::numInputs; i++)
  {
    pinMode(Globals::leds[i], OUTPUT);
    instruments[i]->timing.counts = 0;
    for (int j = 0; j < 8; j++)
    {
      instruments[i]->score.read_rhythm_slot[j] = false;
      instruments[i]->score.set_rhythm_slot[j] = false;
      instruments[i]->topography.a_8[j] = 0;
      instruments[i]->topography.a_16[j] = 0;
      instruments[i]->score.allocated_channel = 0;
    }
    instruments[i]->initialEffect = instruments[i]->effect;
  }
  pinMode(VIBR, OUTPUT);
  pinMode(FOOTSWITCH, INPUT_PULLUP);

  // calculate noise floor:
  calculateNoiseFloor();

  // setup initial values ---------------------------------------------
  instruments[Snare]->pin = A0;
  instruments[Hihat]->pin = A1;
  instruments[Kick]->pin = A2;
  instruments[Tom1]->pin = A3;
  instruments[Tom2]->pin = A4;
  instruments[Standtom1]->pin = A5;
  instruments[Cowbell]->pin = A6;

  // set instrument calibration array
  // values as of 2020-08-27:
  instruments[Snare]->sensitivity.threshold = 180;
  instruments[Snare]->sensitivity.crossings = 12;
  instruments[Hihat]->sensitivity.threshold = 70; // 60
  instruments[Hihat]->sensitivity.crossings = 30; // 20
  instruments[Kick]->sensitivity.threshold = 100;
  instruments[Kick]->sensitivity.crossings = 16;
  instruments[Tom1]->sensitivity.threshold = 200;
  instruments[Tom1]->sensitivity.crossings = 20;
  instruments[Tom2]->sensitivity.threshold = 300;
  instruments[Tom2]->sensitivity.crossings = 18;
  instruments[Standtom1]->sensitivity.threshold = 70;
  instruments[Standtom1]->sensitivity.crossings = 12;
  instruments[Cowbell]->sensitivity.threshold = 80;
  instruments[Cowbell]->sensitivity.crossings = 15;

  instruments[Snare]->effect = TopographyLog;
  instruments[Snare]->initialEffect = TopographyLog;
  instruments[Hihat]->effect = TapTempo;
  instruments[Hihat]->initialEffect = TapTempo;
  instruments[Kick]->effect = TopographyLog;
  instruments[Kick]->initialEffect = TopographyLog;
  instruments[Tom1]->effect = TopographyLog;
  instruments[Tom1]->initialEffect = TopographyLog;
  instruments[Tom2]->effect = TopographyLog;
  instruments[Tom2]->initialEffect = TopographyLog;
  instruments[Standtom1]->effect = TopographyLog;
  instruments[Standtom1]->initialEffect = TopographyLog;
  instruments[Cowbell]->effect = Monitor;
  instruments[Cowbell]->initialEffect = Monitor;

  // setup notes
  for (int i = 0; i < Globals::numInputs; i++)
  {
    instruments[i]->setup_notes({60, 61, 45, 74, 72, 44, 71});          // insert array of MIDI-notes
    instruments[i]->score.active_note = instruments[i]->score.notes[0]; // set active note pointer to first note
  }

  // int cc_chan[] = {50, 0, 0, 25, 71, 50, 0, 0}; // needed in pinAction 5 and 6
  instruments[Snare]->midi.cc_chan = 50;       // amplevel
  instruments[Hihat]->midi.cc_chan = 0;
  instruments[Kick]->midi.cc_chan = 0;
  instruments[Tom1]->midi.cc_chan = 25;      // sustain
  instruments[Tom2]->midi.cc_chan = 71;      // resonance
  instruments[Standtom1]->midi.cc_chan = 50; // amplevel
  instruments[Cowbell]->midi.cc_chan = 0;

  /* channels on mKORG:
   44=cutoff
   71=resonance
   50=amplevel
   23=attack
   25=sustain
   26=release
*/

  Serial.println("-----------------------------------------------");
  Serial.println("calibration values set as follows:");
  Serial.println("instr\tthrshld\tcrosses\tnoiseFloor");
  for (int i = 0; i < Globals::numInputs; i++)
  {
    Serial.print(Globals::DrumtypeToHumanreadable(DrumType(i)));
    Serial.print("\t");
    Serial.print(instruments[i]->sensitivity.threshold);
    Serial.print("\t");
    Serial.print(instruments[i]->sensitivity.crossings);
    Serial.print("\t");
    Serial.println(instruments[i]->sensitivity.noiseFloor);
  }
  Serial.println("-----------------------------------------------");

  // start timers -----------------------------------------------------
  pinMonitor.begin(samplePins, 1000); // sample pin every 1 millisecond

  Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM

  // Debug:
  // tsunami.trackPlayPoly(1, 0, true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
  // tracknum, channel
}

/* --------------------------------------------------------------------- */
/* ------------------------------- LOOP -------------------------------- */
/* --------------------------------------------------------------------- */

void loop()
{

  Globals::tsunami.update();

  // ------------------------- DEBUG AREA -----------------------------
  printNormalizedValues(printNormalizedValues_);

  // --------------------- INCOMING SIGNALS FROM PIEZOS ---------------
  // (define what should happen when instruments are hit)
  for (int i = 0; i < Globals::numInputs; i++)
  {
    if (stroke_detected(i)) // evaluates pins for activity repeatedly
    {
      // ----------------------- perform pin action -------------------
      instruments[i]->trigger(instruments[i], MIDI); // runs trigger function according to instrument's EffectType
      switch (instruments[i]->effect)
      {

      case 8: // create beat-topography in 16-th
        Globals::setInstrumentPrintString(instruments[i]->drumtype, Monitor);
        instruments[i]->topography.a_16[Globals::current_16th_count]++;
        break;

      default:
        break;
      }

      // send instrument stroke to processing:
      // send_to_processing(i);
    }
  } // end main commands loop -----------------------------------------

  /////////////////////////// TIMED ACTIONS ///////////////////////////
  /////////////////////////////////////////////////////////////////////
  // (automatically invoke rhythm-linked actions)
  static int last_beat_pos = 0;
  static boolean toggleLED = true;
  static boolean sendMidiCopy = false;

  static unsigned long vibration_begin = 0;
  static int vibration_duration = 0;

  static boolean already_printed = false;

  noInterrupts();
  Globals::current_beat_pos = Globals::beatCount % 32; // (beatCount increases infinitely)
  sendMidiCopy = Globals::sendMidiClock;      // MIDI flag for Clock to be sent
  interrupts();

  // ---------------------------- send MIDI-Clock on beat
  /* MIDI Clock events are sent at a rate of 24 pulses per quarter note
     one tap beat equals one quarter note
     only one midi clock signal should be send per 24th quarter note
  */
  if (sendMidiCopy)
  {
    Serial2.write(0xF8); // MIDI-clock has to be sent 24 times per quarter note
    noInterrupts();
    Globals::sendMidiClock = false;
    interrupts();
  }

  // DO THINGS ONCE PER 32nd-STEP: ------------------------------------
  // ------------------------------------------------------------------
  if (Globals::current_beat_pos != last_beat_pos)
  {
    // increase 8th note counter:
    if (Globals::current_beat_pos % 4 == 0)
    {
      Globals::current_eighth_count = (Globals::current_eighth_count + 1) % 8;
      toggleLED = !toggleLED;
    }
    digitalWrite(LED_BUILTIN, toggleLED);

    // increase 16th note counter:
    if (Globals::current_beat_pos % 2 == 0)
    {
      Globals::current_16th_count = (Globals::current_16th_count + 1) % 16;
    }

    // -------------------------- PIN ACTIONS: ------------------------
    // ----------------------------------------------------------------
    for (int i = 0; i < Globals::numInputs; i++)
    {
      instruments[i]->perform(instruments[i], MIDI);


      // ---------- MIDI playback according to beat_topography --------
      if (instruments[i]->effect == 8)
      {
        if (Globals::current_16th_count != Globals::last_16th_count) // do this only once per 16th step
        {

          // create overall volume topography of all instrument layers:
          int total_vol[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

          for (int idx = 0; idx < 16; idx++)
          {
            for (int instr = 0; instr < Globals::numInputs; instr++)
            {
              if (instruments[instr]->effect == 8)
                total_vol[idx] += instruments[instr]->topography.a_16[idx];
            }
          }

          // smoothen array:
          // ------------------ create topography and smoothen it -------------
          smoothen_dataArray(total_vol, 3);

          // print volume layer:
          if (!already_printed)
          {
            Serial.print("vol:\t[");

            for (int j = 0; j < 16; j++)
            {
              Serial.print(total_vol[j]);
              if (j < 15)
                Serial.print(",");
            }
            Serial.println("]");
            already_printed = true;
          }

          // ------------ result: change volume and play MIDI ---------
          // ----------------------------------------------------------
          int vol = min(40 + total_vol[Globals::current_16th_count] * 15, 255);

          if (instruments[i]->topography.a_16[Globals::current_16th_count] > 0)
          {
            MIDI.sendControlChange(50, vol, 2);
            MIDI.sendNoteOn(instruments[i]->score.active_note, 127, 2);
          }
          else
          {
            MIDI.sendNoteOff(instruments[i]->score.active_note, 127, 2);
          }

          // Debug:
          Serial.print(Globals::DrumtypeToHumanreadable(DrumType(i)));
          Serial.print(":\t[");
          for (int j = 0; j < 16; j++)
          {
            Serial.print(instruments[i]->topography.a_16[j]);
            if (j < 15)
              Serial.print(",");
          }
          Serial.println("]");
        } // end only once per 16th-step
      }   // end pinAction[8]
    }     // end pin Actions
    // ----------------------------------------------------------------

    ///////////////////////////////////////////////////////////////////
    /////////////////////////// ABSTRACTIONS //////////////////////////
    ///////////////////////////////////////////////////////////////////

    //makeTopo();
    // works like this:
    smoothen_dataArray(&beat_topography_16_);
    // LOTS OF PSEUDOCODE HERE:

    //    beat_topography_16.smoothen(); // → beat_topography.average_smoothened_height
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
    //    when regularity expectations not fulfilled:
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

    // ----------------------------------------------------------------

    /////////////////////// AUXILIARY FUNCTIONS ///////////////////////
    // ---------------------------- vibrate on beat:
    if (Globals::current_beat_pos % 8 == 0) // Globals::current_beat_pos holds 32 → %8 makes 4.
    {
      vibration_begin = millis();
      vibration_duration = 50;
      digitalWrite(VIBR, HIGH);
    }

    // ----------------------------- draw play log to console
    print_to_console(String(millis()));
    print_to_console("\t");
    // Serial.print(Globals::current_eighth_count + 1); // if you want to print 8th-steps only
    print_to_console(Globals::current_beat_pos);
    print_to_console("\t");
    // Serial.print(Globals::current_beat_pos / 4);
    // Serial.print("\t");
    // Serial.print(Globals::current_eighth_count);
    for (int i = 0; i < Globals::numInputs; i++)
    {
      print_to_console(Globals::output_string[i]);
      Globals::output_string[i] = "\t";
    }
    println_to_console("");

    // Debug: play MIDI note on quarter notes
    //    if (Globals::current_beat_pos % 8 == 0)
    //    MIDI.sendNoteOn(57, 127, 2);
    //    else
    //    MIDI.sendNoteOff(57, 127, 2);

  } // end of (32nd-step) TIMED ACTIONS
  // ------------------------------------------------------------------

  ///////////////////////////// tidy up ///////////////////////////////
  last_beat_pos = Globals::current_beat_pos;
  Globals::last_eighth_count = Globals::current_eighth_count;
  Globals::last_16th_count = Globals::current_16th_count;
  already_printed = false;

  // check footswitch -------------------------------------------------
  checkFootSwitch();

  // turn off vibration and MIDI notes --------------------------------
  if (millis() > vibration_begin + vibration_duration)
    digitalWrite(VIBR, LOW);

  for (int i = 0; i < Globals::numInputs; i++)
    if (millis() > instruments[i]->score.last_notePlayed + 200 && instruments[i]->effect != Swell) // pinAction 5 turns notes off in swell_beat()
      MIDI.sendNoteOff(instruments[i]->score.active_note, 127, 2);
}
// --------------------------------------------------------------------