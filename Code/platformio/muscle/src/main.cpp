/*
   ------------------------------------
   September 2020
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

Tsunami tsunami;
// MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); // for Serial-specific usage
midi::MidiInterface<HardwareSerial> MIDI((HardwareSerial&)Serial2); // same as // MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);



// ----------------------------- pins ---------------------------------
static const uint8_t numInputs = 7;
// static const uint8_t pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
Instrument *instruments[numInputs];

// static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN1, LED_BUILTIN2, LED_BUILTIN3}; // array when using SPRESENSE
static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN};

// ------------------------- Debug variables --------------------------
boolean use_responsiveCalibration = false;
boolean printStrokes = true;
boolean printNormalizedValues_ = false;
boolean do_print_to_console = true;
boolean do_send_to_processing = false;
String output_string[numInputs];

// ------------------ variables for interrupt timers ------------------
IntervalTimer pinMonitor;  // reads pins every 1 ms
// IntervalTimer masterClock; // 1 bar
// volatile int counts[numInputs];
// volatile unsigned long lastPinActiveTime[numInputs];
// volatile unsigned long firstPinActiveTime[numInputs];

// ------------------ timing and rhythm tracking ------------------
int countsCopy[numInputs];
int current_beat_pos = 0; // always stores the current position in the beat

// int tapInterval = 500; // 0.5 s per beat for 120 BPM
// int current_BPM = 120;

// // ----------------------------- timer counter ---------------------------------
// volatile unsigned long masterClockCount = 0; // 4*32 = 128 masterClockCount per cycle
// volatile unsigned long beatCount = 0;
// // volatile int bar_step; // 0-32
// volatile int currentStep; // 0-32
// int next_beatCount = 0;   // will be reset when timer restarts
// volatile boolean sendMidiClock = false;

// ----------------- MUSICAL AND PERFORMATIVE PARAMETERS --------------

// boolean read_rhythm_slot[numInputs][8];
// boolean set_rhythm_slot[numInputs][8];
// int beat_topography_8[numInputs][8];
// int beat_topography_16[numInputs][16];

const int LOG_BEATS = 0;
const int HOLD_CC = 1;
const int RESET_TOPO = 2; // resets beat_topography (of all instruments)
const int FOOTSWITCH_MODE = 2;

// int notes_list[] = {60, 61, 45, 74, 72, 44, 71};
int cc_chan[] = {50, 0, 0, 25, 71, 50, 0, 0}; // needed in pinAction 5 and 6
/* channels on mKORG:
   44=cutoff
   71=resonance
   50=amplevel
   23=attack
   25=sustain
   26=release
*/
// int pinAction[] = {8, 4, 8, 8, 8, 8, 1, 0}; // array to be changed within code loop.

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
// int initialPinAction[numInputs];
int allocated_track[numInputs];                   // tracks will be allocated in tsunami_beat_playback
int allocated_channels[] = {0, 0, 0, 0, 0, 0, 0}; // channels to send audio from tsunami to

// hard-coded list of BPMs of tracks stored on Tsunami's SD card.
// TODO: somehow make BPM accessible from file title
const float track_bpm[256] =
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
  for (int pinNum = 0; pinNum < numInputs; pinNum++)
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
// void samplePins()
// {
//   // ------------------------- read all pins -----------------------------------
//   for (int pinNum = 0; pinNum < numInputs; pinNum++)
//   {
//     if (pinValue(pinNum) > instruments[pinNum]->sensitivity.threshold)
//     {
//       if (counts[pinNum] < 1)
//         firstPinActiveTime[pinNum] = millis();
//       lastPinActiveTime[pinNum] = millis();
//       counts[pinNum]++;
//     }
//   }
// }
// -----------------------------------------------------------------------------

// void masterClockTimer()
// {
//   /*
//     timing              fraction          bar @ 120 BPM    bar @ 180 BPM
//     1 bar             = 1               = 2 s       | 1.3 s
//     1 beatCount       = 1/32 bar        = 62.5 ms   | 41.7 ms
//     stroke precision  = 1/4 beatCount   = 15.625 ms | 10.4166 ms


//     |     .-.
//     |    /   \         .-.
//     |   /     \       /   \       .-.     .-.     _   _
//     +--/-------\-----/-----\-----/---\---/---\---/-\-/-\/\/---
//     | /         \   /       \   /     '-'     '-'
//     |/           '-'         '-'
//     |--------2 oscil---------|
//     |------snare = 10 ms-----|

//   */

//   masterClockCount++; // will rise infinitely

//   // ------------ 1/32 increase in 4 full precisionCounts -----------
//   if (masterClockCount % 4 == 0)
//   {
//     beatCount++; // will rise infinitely
//   }

//   // evaluate current position of beat in bar for stroke precision
//   // 2020-09-07: this doesn't help and it's also not working...
//   // if ((masterClockCount % 4) >= next_beatCount - 2)
//   // {
//   //   currentStep = next_beatCount;
//   //   next_beatCount += 4;
//   // }

//   // prepare MIDI clock:
//   sendMidiClock = (((masterClockCount % 4) / 3) % 4 == 0);

// }
//   // ---------------------------------------------------------------------------

/* --------------------------------------------------------------------- */
/* ----------------------------- FUNCTIONS ----------------------------- */
/* --------------------------------------------------------------------- */

////////////////////////////// CALCULATE NOISEFLOOR ///////////////////
///////////////////////////////////////////////////////////////////////
void calculateNoiseFloor()
{
  // calculates the average noise floor out of 400 samples from all inputs

  int led_idx = 0;
  for (int pinNum = 0; pinNum < numInputs; pinNum++)
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
        digitalWrite(leds[pinNum], toggleState);
        toggleState = !toggleState;
      }
      totalSamples += analogRead(instruments[pinNum]->pin);
    }
    instruments[pinNum]->sensitivity.noiseFloor = totalSamples / 400;
    digitalWrite(leds[pinNum], LOW);
    led_idx++;
    Serial.print("noiseFloor = ");
    Serial.println(instruments[pinNum]->sensitivity.noiseFloor);
  }

  for (int i = 0; i < numInputs; i++) // turn LEDs off again
  {
    digitalWrite(leds[i], LOW);
    output_string[i] = "\t";
  }
}
// --------------------------------------------------------------------

////////////////////////////////// FOOT SWITCH ////////////////////////
///////////////////////////////////////////////////////////////////////
EffectsType lastEffect[numInputs];

int swell_val[numInputs];
boolean footswitch_is_pressed = false;

void footswitch_pressed()
{
  switch (FOOTSWITCH_MODE)
  {
  case (LOG_BEATS):
    // set pinMode of all instruments to 3 (record what is being played)
    for (int i = 0; i < numInputs; i++)
    {
      lastEffect[i] = instruments[i]->effect; // TODO: lastPinAction seems to be overwritten quickly, so it will not be able to return to its former state. fix this!
      instruments[i]->effect = FootSwitchLooper;                // TODO: not for Cowbell?
      for (int j = 0; j < 8; j++)
        instruments[i]->score.set_rhythm_slot[j] = false; // reset entire record
    }
    break;

  case (HOLD_CC): // prevents swell_vals to be changed in swell_beat()
    footswitch_is_pressed = true;
    break;

  case (RESET_TOPO): // resets beat_topography (for all instruments)
    for (int i = 0; i < numInputs; i++)
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
    for (int i = 0; i < numInputs; i++)
      instruments[i]->effect = instruments[i]->initialEffect;
    break;

  case (HOLD_CC):
    footswitch_is_pressed = false;
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
  //static boolean lastPinAction[numInputs];

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

///////////////////// SET STRING FOR PLAY LOGGING /////////////////////
///////////////////////////////////////////////////////////////////////

// int swell_val[numInputs]; // this should be done in the swell section, but is needed in print section already... :/

void setInstrumentPrintString(int incoming_i, int incoming_pinAction)
{
  switch (incoming_pinAction)
  {

  case 1: // monitor: just print what is being played
    if (incoming_i == Kick)
      output_string[incoming_i] = "■\t"; // Kickdrum
    else if (incoming_i == Cowbell)
      output_string[incoming_i] = "▲\t"; // Crash
    else if (incoming_i == Standtom1)
      output_string[incoming_i] = "□\t"; // Standtom
    else if (incoming_i == Standtom2)
      output_string[incoming_i] = "O\t"; // Standtom
    else if (incoming_i == Hihat)
      output_string[incoming_i] = "x\t"; // Hi-Hat
    else if (incoming_i == Tom1)
      output_string[incoming_i] = "°\t"; // Tom 1
    else if (incoming_i == Snare)
      output_string[incoming_i] = "※\t"; // Snaredrum
    else if (incoming_i == Tom2)
      output_string[incoming_i] = "o\t"; // Tom 2
    else if (incoming_i == Ride)
      output_string[incoming_i] = "xx\t"; // Ride
    else if (incoming_i == Crash1)
      output_string[incoming_i] = "-X-\t"; // Crash
    else if (incoming_i == Crash2)
      output_string[incoming_i] = "-XX-\t"; // Crash
    break;

  case 2: // toggle beat slot
    if (incoming_i == Kick)
      output_string[incoming_i] = "■\t"; // Kickdrum
    else if (incoming_i == Cowbell)
      output_string[incoming_i] = "▲\t"; // Crash
    else if (incoming_i == Standtom1)
      output_string[incoming_i] = "□\t"; // Standtom
    else if (incoming_i == Standtom2)
      output_string[incoming_i] = "O\t"; // Standtom
    else if (incoming_i == Hihat)
      output_string[incoming_i] = "x\t"; // Hi-Hat
    else if (incoming_i == Tom1)
      output_string[incoming_i] = "°\t"; // Tom 1
    else if (incoming_i == Snare)
      output_string[incoming_i] = "※\t"; // Snaredrum
    else if (incoming_i == Tom2)
      output_string[incoming_i] = "o\t"; // Tom 2
    else if (incoming_i == Ride)
      output_string[incoming_i] = "xx\t"; // Ride
    else if (incoming_i == Crash1)
      output_string[incoming_i] = "-X-\t"; // Crash
    else if (incoming_i == Crash2)
      output_string[incoming_i] = "-XX-\t"; // Crash
    break;

  case 3: // add an ! if pinAction == 3 (replay logged rhythm)
    if (incoming_i == Kick)
      output_string[incoming_i] = "!■\t"; // Kickdrum
    else if (incoming_i == Cowbell)
      output_string[incoming_i] = "!▲\t"; // Crash
    else if (incoming_i == Standtom1)
      output_string[incoming_i] = "!□\t"; // Standtom
    else if (incoming_i == Standtom2)
      output_string[incoming_i] = "!O\t"; // Standtom
    else if (incoming_i == Hihat)
      output_string[incoming_i] = "!x\t"; // Hi-Hat
    else if (incoming_i == Tom1)
      output_string[incoming_i] = "!°\t"; // Tom 1
    else if (incoming_i == Snare)
      output_string[incoming_i] = "!※\t"; // Snaredrum
    else if (incoming_i == Tom2)
      output_string[incoming_i] = "!o\t"; // Tom 2
    else if (incoming_i == Ride)
      output_string[incoming_i] = "!xx\t"; // Ride
    else if (incoming_i == Crash1)
      output_string[incoming_i] = "!-X-\t"; // Crash
    else if (incoming_i == Crash2)
      output_string[incoming_i] = "!-XX-\t"; // Crash
    break;

    // case 5: // print swell_val for repeated MIDI notes in "swell" mode
    //   output_string[incoming_i] = swell_val[incoming_i];
    //   output_string[incoming_i] += "\t";
    //   break;
  }
}

// --------------------------------------------------------------------

// ///////////////////////// STROKE DETECTION /////////////////////////
// ////////////////////////////////////////////////////////////////////
boolean stroke_detected(int instr)
{
  static unsigned long lastPinActiveTimeCopy[numInputs];
  // static unsigned long firstPinActiveTimeCopy[numInputs];
  // static int lastValue[numInputs];    // for LED toggle
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

// //////////////////// SOUND SWELL: INITIALIZATION ///////////////////
// ////////////////////////////////////////////////////////////////////

/* SOUND SWELL ALGORITHM: ---------------------------------------------

  RECORD:

  -------X-------X-------X-------X------- hits
  -------|-------|-------|-------|-------
  -------1-------2-------3-------4------- num_of_swell_taps
  -------|-------|-------|-------|-------
  ------502-----510-----518-----526------ beatCount (beatPos)
  -------|-------|-------|-------|-------
  -------|-------|------prev----curr-----
  -------|-------|-------|-------|-------
  -------|-------8-------16------32------ swell_beatPos_sum = sum + (curr - prev)
  -------|---8---|---8---|---8---|------- swell_stroke_interval = sum/num
  ------v++-----v++-----v++-----v++------ increase swell_val


  PLAY:

  -------|-------|-------|-------|-------
  -------0-------1-------2-------3------- beatStep
  -------|-------|-------|-------|-------
  -------v-----(v↓)----(v↓)----(v↓)------ decrease swell_val
  -------🎵-------🎵-------🎵------🎵------- play MIDI note

  ---------------------------------------------------------------------*/

int num_of_swell_taps[numInputs];     // will be used in both swell_rec() and swell_beat(). serves as swell_val for MIDI notes.
int swell_stroke_interval[numInputs]; // will be needed for timed replay
int swell_state[numInputs];
unsigned long swell_beatPos_sum[numInputs];
int swell_beatStep[numInputs]; // increases with beatCount and initiates action.

void swell_init() // initialize arrays for swell functions (run in setup())
{
  for (int i = 0; i < numInputs; i++)
  {
    swell_state[i] = 1; // waits for first tap
    num_of_swell_taps[i] = 0;
    swell_val[i] = 10;
    swell_beatPos_sum[i] = 0;
    swell_beatStep[i] = 0;
  }
}
// --------------------------------------------------------------------

// -------------------- SOUND SWELL: RECORD STROKES -------------------
// --------------------------------------------------------------------

void swell_rec(int instr) // remembers beat stroke position
{
  /* works pretty much just like the tapTempo, but repeats the triggered drums on external MIDI instrument (-> swell_beat() in TIMED INTERVALS) */
  static unsigned long previous_swell_beatPos[numInputs];
  // static unsigned long lastSwellRec = 0;

  if (swell_state[instr] == 1) // first hit
  {
    // grab the current beat counter
    noInterrupts();
    previous_swell_beatPos[instr] = Globals::beatCount;
    interrupts();

    // start MIDI note and proceed to next state
    swell_state[instr] = 2;
    MIDI.sendNoteOn(instruments[instr]->score.active_note, 127, 2);

    // lastSwellRec = millis();
  }

  else if (swell_state[instr] == 2) // second hit
  {
    if (!footswitch_is_pressed)
    {
      num_of_swell_taps[instr]++;
      swell_val[instr] += 4;                         // ATTENTION: must rise faster than it decreases! otherwise swell resets right away.
      swell_val[instr] = min(swell_val[instr], 127); // max swell_val = 127
    }

    unsigned long current_swell_beatPos;

    noInterrupts();
    current_swell_beatPos = Globals::beatCount;
    interrupts();

    // calculate diff and interval:
    if ((current_swell_beatPos - previous_swell_beatPos[instr]) <= 32) // only do this if interval was not too long
    {
      // get diff to first (how many beats were in between?):
      swell_beatPos_sum[instr] += (current_swell_beatPos - previous_swell_beatPos[instr]);
      // average all hits and repeat at that rate:
      float a = float(swell_beatPos_sum[instr]) / float(num_of_swell_taps[instr]);
      a += 0.5;                              // rounds up or down
      swell_stroke_interval[instr] = int(a); // round down
      previous_swell_beatPos[instr] = current_swell_beatPos;
    }
  }
}
// --------------------------------------------------------------------

// -------------------- SOUND SWELL: REPLAY STROKES -------------------
// --------------------------------------------------------------------

void swell_perform(int instr, int perform_action) // updates once a 32nd-beat-step
{
  if (swell_state[instr] == 2)
  {
    // remember moment of update in beat:
    //    noInterrupts();
    //    unsigned long updateMoment = beatCount;
    //    interrupts();

    // ready to play MIDI again?
    // increase swell_beatStep and modulo interval
    swell_beatStep[instr] = (swell_beatStep[instr] + 1) % swell_stroke_interval[instr];

    if (swell_beatStep[instr] == 0) // on swell beat
    {

      // Debug print:
      //output_string[instr] = String(swell_val[instr]);
      //      output_string[instr] = "[";
      //      output_string[instr] += String(swell_stroke_interval[instr]);
      //      output_string[instr] += "] ";
      //      output_string[instr] += num_of_swell_taps[instr];
      //      output_string[instr] += "/";
      //      output_string[instr] += swell_beatPos_sum[instr];
      //      output_string[instr] += " (";
      output_string[instr] = String(swell_val[instr]);
      //      output_string[instr] += ") ";
      output_string[instr] += "\t";

      if (perform_action == 5)
      {
        if (!footswitch_is_pressed)
          MIDI.sendControlChange(cc_chan[instr], swell_val[instr], 2);
      }
      /* channels on mKORG: 44=cutoff, 50=amplevel, 23=attack, 25=sustain, 26=release
        finding the right CC# on microKORG: (manual p.61):
        1. press SHIFT + 5
        2. choose parameter to find out via EDIT SELECT 1 & 2
        (3. reset that parameter, if you like) */

      // MIDI.sendNoteOn(notes_list[instr], 127, 2); // also play a note on each hit?
      else if (perform_action == 7)
      {
        if (tsunami.isTrackPlaying(allocated_track[instr]))
        {
          static int trackLevel = 0;
          static int previousTracklevel = 0;
          trackLevel = min(-40 + swell_val[instr], 0);
          if (trackLevel != previousTracklevel)
            tsunami.trackFade(allocated_track[instr], trackLevel, 100, false); // fade smoothly within 100 ms
          previousTracklevel = trackLevel;
        }
      }
      // decrease swell_val:
      if (swell_val[instr] > 0)
      {
        if (!footswitch_is_pressed)
          swell_val[instr]--;
      }
      else // reset swell if swell_val == 0:
      {
        swell_state[instr] = 1; // waits for first tap
        num_of_swell_taps[instr] = 0;
        swell_val[instr] = 10;
        swell_beatPos_sum[instr] = 0;
        swell_beatStep[instr] = 0;
        MIDI.sendNoteOff(instruments[instr]->score.active_note, 127, 2);
      }
    }
  }
}
// --------------------------------------------------------------------

////////////////////////////// TAP TEMPO //////////////////////////////
///////////////////////////////////////////////////////////////////////

// void getTapTempo()
// {
//   static unsigned long timeSinceFirstTap = 0;
//   static int tapState = 1;
//   static int num_of_taps = 0;
//   static int clock_sum = 0;

//   switch (tapState)
//   {
//     //    case 0: // this is for activation of tap tempo listen
//     //      tapState = 1;
//     //      break;

//   case 1:                                     // first hit
//     if (millis() > timeSinceFirstTap + 10000) // reinitiate tap if not used for ten seconds
//     {
//       num_of_taps = 0;
//       clock_sum = 0;
//       Serial.println("-----------TAP RESET!-----------\n");
//     }
//     timeSinceFirstTap = millis(); // record time of first hit
//     tapState = 2;                 // next: wait for second hit

//     break;

//   case 2: // second hit

//     if (millis() < timeSinceFirstTap + 2000) // only record tap if interval was not too long
//     {
//       num_of_taps++;
//       clock_sum += millis() - timeSinceFirstTap;
//       tapInterval = clock_sum / num_of_taps;
//       Serial.print("new tap Tempo is ");
//       Serial.print(60000 / tapInterval);
//       Serial.print(" bpm (");
//       Serial.print(tapInterval);
//       Serial.println(" ms interval)");

//       current_BPM = 60000 / tapInterval;
//       tapState = 1;

//       masterClock.begin(masterClockTimer, tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
//     }

//     if (timeSinceFirstTap > 2000) // forget tap if time was too long
//     {
//       tapState = 1;
//       // Serial.println("too long...");
//     }
//     // }
//     break;
//   }
// }
// --------------------------------------------------------------------

//////////////////// TSUNAMI BEAT-LINKED PLAYBACK /////////////////////
///////////////////////////////////////////////////////////////////////

/* ------------- Tsunami Beat-linked Playback algorithm: --------------


  X-----------X---X--------------- 32nd Beat
  [1,  0,  0,  1,  1,  0,  0,  0  ] 1. iteration
  |-----------|---|---------------
  +1, +0, +0, +1, +1, +0, +0, +0    changes between iterations
  |-----------|---|---------------
  [2,  0,  0,  2,  2,  0,  0,  0  ] 2. iteration
  |-----------|---|---------------
  +0. +1, +0, +1, +1, +0, +0, +0    changes, unprecisely played
  ----|-------|---|---------------
  [2,  1,  0,  3,  3,  0,  0,  0  ] 3. iteration
  |-----------|---|---------------
  4 + 1 + 0 + 9 + 9 + 0 + 0 + 0 = 23 beat_topo_squared_sum
  |-----------|---|---------------

  4/23 = 0.174
          ratio = 4:1 = 0.25
  1/23 = 0.043
          ratio = 9:1 = 0.111 --> beat_topo_entries -= 1; beat_topo[i] = 0
  9/23 = 0.391

  beat_topo_regular_sum = 2 + 1 + 3 + 3 = 9
  smoothing: beat_topo_entries = 4 - 1 = 3
  beat_topo_average_smooth = int(9/3 + 0.5) = 3

  ---------------------------------------------------------------------*/

void tsunami_beat_playback(int instr, int current_beat_in)
{
  // ------------------ create topography and smoothen it -------------
  // smoothen_dataArray(beat_topography_8, instr, 3)
  int beat_topo_entries = 0;
  int beat_topo_squared_sum = 0;
  int beat_topo_regular_sum = 0;

  // TODO: compress 16-bit to 8-bit topography

  // count entries and create squared sum:
  for (int j = 0; j < 8; j++)
  {
    if (instruments[instr]->topography.a_8[j] > 0)
    {
      beat_topo_entries++;
      beat_topo_squared_sum += instruments[instr]->topography.a_8[j] * instruments[instr]->topography.a_8[j];
      beat_topo_regular_sum += instruments[instr]->topography.a_8[j];
    }
  }

  beat_topo_regular_sum = beat_topo_regular_sum / beat_topo_entries;

  // calculate site-specific fractions (squared):
  float beat_topo_squared_frac[8];
  for (int j = 0; j < 8; j++)
    beat_topo_squared_frac[j] =
        float(instruments[instr]->topography.a_8[j]) / float(beat_topo_squared_sum);

  // get highest frac:
  float highest_frac = 0;
  for (int j = 0; j < 8; j++)
    highest_frac = (beat_topo_squared_frac[j] > highest_frac) ? beat_topo_squared_frac[j] : highest_frac;

  // get "topography height":
  // divide highest with other entries and omit entries if ratio > 3:
  for (int j = 0; j < 8; j++)
    if (beat_topo_squared_frac[j] > 0)
      if (highest_frac / beat_topo_squared_frac[j] > 3 || beat_topo_squared_frac[j] / highest_frac > 3)
      {
        instruments[instr]->topography.a_8[j] = 0;
        beat_topo_entries -= 1;
        Serial.print(Globals::DrumtypeToHumanreadable(DrumType(j)));
        Serial.print(": REDUCED VAL AT POS ");
        Serial.println(j);
      }

  int beat_topo_average_smooth = 0;
  // assess average topo sum for loudness
  for (int j = 0; j < 8; j++)
    beat_topo_regular_sum += instruments[instr]->topography.a_8[j];
  beat_topo_average_smooth = int((float(beat_topo_regular_sum) / float(beat_topo_entries)) + 0.5);

  // TODO: reduce all params if not played for long.

  int tracknum = 0;               // Debug
  if (beat_topo_regular_sum >= 3) // only initiate playback if average of entries > certain threshold.
  {

    // find right track from database:
    //int tracknum = 0;
    for (int j = 0; j < 8; j++)
    {
      if (instruments[instr]->topography.a_8[j] > 0)
      {
        if (j == 0)
          tracknum += 128;
        if (j == 1)
          tracknum += 64;
        if (j == 2)
          tracknum += 32;
        if (j == 3)
          tracknum += 16;
        if (j == 4)
          tracknum += 8;
        if (j == 5)
          tracknum += 4;
        if (j == 6)
          tracknum += 2;
        if (j == 7)
          tracknum += 1;
      }
    }
    allocated_track[instr] = tracknum; // save for use in other functions

    // set loudness and fade:
    //int trackLevel = min(-40 + (beat_topo_average_smooth * 5), 0);
    int trackLevel = 0;                                          // Debug
    tsunami.trackFade(tracknum, trackLevel, Globals::tapInterval, false); // fade smoothly within length of a quarter note

    // TODO: set track channels for each instrument according to output
    // output A: speaker on drumset
    // output B: speaker in room (PA)
    // cool effects: let sounds walk through room from drumset to PA

    // --------------------------- play track -------------------------
    if (!tsunami.isTrackPlaying(tracknum) && current_beat_in == 0)
    {
      // set playback speed according to current_BPM:
      int sr_offset;
      float r = Globals::current_BPM / float(track_bpm[tracknum]);
      Serial.print("r = ");
      Serial.println(r);
      if (!(r > 2) && !(r < 0.5))
      {
        // samplerateOffset scales playback speeds from 0.5 to 1 to 2
        // and maps to -32768 to 0 to 32767
        sr_offset = (r >= 1) ? 32767 * (r - 1) : -32768 + 32768 * 2 * (r - 0.5);
        sr_offset = int(sr_offset);
        Serial.print("sr_offset = ");
        Serial.println(sr_offset);
      }
      else
      {
        sr_offset = 0;
      }

      //int channel = 0;                              // Debug
      tsunami.samplerateOffset(allocated_channels[instr], sr_offset); // TODO: link channels to instruments
      tsunami.trackGain(tracknum, trackLevel);
      tsunami.trackPlayPoly(tracknum, allocated_channels[instr], true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
      Serial.print("starting to play track ");
      Serial.println(tracknum);
    } // track playing end
  }   // threshold end

  // Debug:
  Serial.print("[");
  for (int i = 0; i < 8; i++)
  {
    Serial.print(instruments[instr]->topography.a_8[i]);
    if (i < 7)
      Serial.print(", ");
  }
  Serial.print("]\t");
  //  Serial.print(beat_topo_entries);
  //  Serial.print("\t");
  //  Serial.print(beat_topo_squared_sum);
  //  Serial.print("\t");
  //  Serial.print(beat_topo_regular_sum);
  //  Serial.print("\t");
  //  Serial.print(beat_topo_average_smooth);
  Serial.print("\t");
  int trackLevel = min(-40 + (beat_topo_average_smooth * 5), 0);
  Serial.print(trackLevel);
  Serial.print("dB\t->");
  Serial.println(tracknum);
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
      for (int i = 0; i < numInputs; i++)
      {
        // static int countsCopy[numInputs];
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
void smoothen_dataArray(int input_array[numInputs][8], int instr_in, int threshold_to_omit_entry)
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
  for (int i = 0; i < numInputs; i++)
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
  tsunami.start(); // Tsunami startup at 57600. ATTENTION: Serial Channel is selected in Tsunami.h !!!
  delay(100);
  tsunami.stopAllTracks(); // in case Tsunami was already playing.
  tsunami.samplerateOffset(0, 0);
  tsunami.setReporting(true); // Enable track reporting from the Tsunami
  delay(100);                 // some time for Tsunami to respond with version string

  //------------------------ initialize pins and arrays ------------------------
  for (int i = 0; i < numInputs; i++)
  {
    pinMode(leds[i], OUTPUT);
    instruments[i]->timing.counts = 0;
    for (int j = 0; j < 8; j++)
    {
      instruments[i]->score.read_rhythm_slot[j] = false;
      instruments[i]->score.set_rhythm_slot[j] = false;
      instruments[i]->topography.a_8[j] = 0;
      instruments[i]->topography.a_16[j] = 0;
    }
    instruments[i]->initialEffect = instruments[i]->effect;
  }
  swell_init();
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
  for (int i = 0; i < numInputs; i++)
  {
    instruments[i]->setup_notes({60, 61, 45, 74, 72, 44, 71}); // insert array of MIDI-notes
    instruments[i]->score.active_note = instruments[i]->score.notes[0]; // set active note pointer to first note
  }

  Serial.println("-----------------------------------------------");
  Serial.println("calibration values set as follows:");
  Serial.println("instr\tthrshld\tcrosses\tnoiseFloor");
  for (int i = 0; i < numInputs; i++)
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
  // static int current_eighth_count = 0; // overflows at current_beat_pos % 8
  // static int current_16th_count = 0;   // overflows at current_beat_pos % 2
  // static int last_eighth_count = 0;    // stores last eightNoteCount for comparison
  // static int last_16th_count = 0;      // stores last eightNoteCount for comparison
  // static unsigned long lastNotePlayed[numInputs];

  tsunami.update();

  // ------------------------- DEBUG AREA -----------------------------
  printNormalizedValues(printNormalizedValues_);

  // --------------------- INCOMING SIGNALS FROM PIEZOS ---------------
  // (define what should happen when instruments are hit)
  for (int i = 0; i < numInputs; i++)
  {
    if (stroke_detected(i)) // evaluates pins for activity repeatedly
    {
      // ----------------------- perform pin action -------------------
      instruments[i]->trigger(instruments[i], MIDI); // runs trigger function according to instrument's EffectType
      switch (instruments[i]->effect)
      {
        // case 0:
        //   MIDI.sendNoteOn(notes_list[i], 127, 2);
        //   lastNotePlayed[i] = millis();
        //   break;

      // case 1: // monitor: just print what is being played.
      //   if (printStrokes)
      //   {
      //     setInstrumentPrintString(i, instruments[i]->effect);
      //   }
      //   break;

      // case 2: // toggle beat slot
      //   if (printStrokes)
      //     setInstrumentPrintString(i, instruments[i]->effect);
      //   read_rhythm_slot[i][Globals::current_eighth_count] = !read_rhythm_slot[i][Globals::current_eighth_count];
      //   break;

      // case 3: // record what is being played and replay it later
      //   if (printStrokes)
      //     setInstrumentPrintString(i, instruments[i]->effect);
      //   set_rhythm_slot[i][Globals::current_eighth_count] = true;
      //   break;

      // case 4: // tapTempo
        // getTapTempo();
        // break;

      case 5: // "swell"
        setInstrumentPrintString(i, instruments[i]->effect);
        swell_rec(i);
        break;

      case 6: // Tsunami beat-linked pattern
        setInstrumentPrintString(i, 1);
        instruments[i]->topography.a_8[Globals::current_eighth_count]++;
        break;

      case 7: // swell-effect for loudness on field recordings (use on cymbals e.g.)
        // TODO: UNTESTED! (2020-10-09)
        setInstrumentPrintString(i, instruments[i]->effect);
        swell_rec(i);
        break;

      case 8: // create beat-topography in 16-th
        setInstrumentPrintString(i, 1);
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
  current_beat_pos = Globals::beatCount % 32; // (beatCount increases infinitely)
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
  if (current_beat_pos != last_beat_pos)
  {
    // increase 8th note counter:
    if (current_beat_pos % 4 == 0)
    {
      Globals::current_eighth_count = (Globals::current_eighth_count + 1) % 8;
      toggleLED = !toggleLED;
    }
    digitalWrite(LED_BUILTIN, toggleLED);

    // increase 16th note counter:
    if (current_beat_pos % 2 == 0)
    {
      Globals::current_16th_count = (Globals::current_16th_count + 1) % 16;
    }

    // -------------------------- PIN ACTIONS: ------------------------
    // ----------------------------------------------------------------
    for (int i = 0; i < numInputs; i++)
    {
      // instruments[i]->perform(instruments[i]->effect);
      // --------------------------- SWELL: ---------------------------
      if (instruments[i]->effect == Swell || instruments[i]->effect == CymbalSwell)
        swell_perform(i, instruments[i]->effect); // ...updates once a 32nd-beat-step

      else if (instruments[i]->effect == FootSwitchLooper) // set rhythm slots to play MIDI notes:
        instruments[i]->score.read_rhythm_slot[Globals::current_eighth_count] =  instruments[i]->score.set_rhythm_slot[Globals::current_eighth_count];

      // ----------- (pinActions 2 and 3): send MIDI notes ------------
      else if (instruments[i]->effect == ToggleRhythmSlot || instruments[i]->effect == FootSwitchLooper)
      {
        if (Globals::current_eighth_count != Globals::last_eighth_count) // in 8th-interval
        {
          if ( instruments[i]->score.read_rhythm_slot[Globals::current_eighth_count])
          {
            setInstrumentPrintString(i, 3);
            MIDI.sendNoteOn(instruments[i]->score.active_note, 127, 2);
          }
          else
            MIDI.sendNoteOff(instruments[i]->score.active_note, 127, 2);
        }
      }

      // ----------------------- TSUNAMI PLAYBACK: --------------------
      else if (instruments[i]->effect == 6)
      {
        tsunami_beat_playback(i, current_beat_pos);
      }

      // ---------- MIDI playback according to beat_topography --------
      if (instruments[i]->effect == 8)
      {
        if (Globals::current_16th_count != Globals::last_16th_count) // do this only once per 16th step
        {

          // create overall volume topography of all instrument layers:
          int total_vol[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

          for (int idx = 0; idx < 16; idx++)
          {
            for (int instr = 0; instr < numInputs; instr++)
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
    if (current_beat_pos % 8 == 0) // current_beat_pos holds 32 → %8 makes 4.
    {
      vibration_begin = millis();
      vibration_duration = 50;
      digitalWrite(VIBR, HIGH);
    }

    // ----------------------------- draw play log to console
    print_to_console(String(millis()));
    print_to_console("\t");
    // Serial.print(Globals::current_eighth_count + 1); // if you want to print 8th-steps only
    print_to_console(current_beat_pos);
    print_to_console("\t");
    // Serial.print(current_beat_pos / 4);
    // Serial.print("\t");
    // Serial.print(Globals::current_eighth_count);
    for (int i = 0; i < numInputs; i++)
    {
      print_to_console(output_string[i]);
      output_string[i] = "\t";
    }
    println_to_console("");

    // Debug: play MIDI note on quarter notes
    //    if (current_beat_pos % 8 == 0)
    //    MIDI.sendNoteOn(57, 127, 2);
    //    else
    //    MIDI.sendNoteOff(57, 127, 2);

  } // end of (32nd-step) TIMED ACTIONS
  // ------------------------------------------------------------------

  ///////////////////////////// tidy up ///////////////////////////////
  last_beat_pos = current_beat_pos;
  Globals::last_eighth_count = Globals::current_eighth_count;
  Globals::last_16th_count = Globals::current_16th_count;
  already_printed = false;

  // check footswitch -------------------------------------------------
  checkFootSwitch();

  // turn off vibration and MIDI notes --------------------------------
  if (millis() > vibration_begin + vibration_duration)
    digitalWrite(VIBR, LOW);

  for (int i = 0; i < numInputs; i++)
    if (millis() > instruments[i]->score.last_notePlayed + 200 && instruments[i]->effect != Swell) // pinAction 5 turns notes off in swell_beat()
      MIDI.sendNoteOff(instruments[i]->score.active_note, 127, 2);
}
// --------------------------------------------------------------------
