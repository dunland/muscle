/*
  logs what is being played on the drum set.
  1. define pin variables according to how they are plugged in to the MCU
  2. run sketch and show monitor (CTRL+m)
   ------------------------------------
   August 2020
   by David Unland david[at]unland[dot]eu
   ------------------------------------
   ------------------------------------

*/

/* --------------------------------------------------------------------- */
/* ------------------------------- GLOBAL ------------------------------ */
/* --------------------------------------------------------------------- */
// #include <MIDI.h>

// MIDI_CREATE_DEFAULT_INSTANCE();
// MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);


// ----------------------------- pins ---------------------------------
static const uint8_t numInputs = 5;
static const uint8_t pins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int SNARE = 6;
const int HIHAT = 4;
const int TOM1 = 5;
const int TOM2 = 7;
const int STANDTOM1 = 1;
const int STANDTOM2 = 2;
const int RIDE = 8;
const int CRASH1 = 8;
const int CRASH2 = 8;
const int KICK = 0;
// static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN1, LED_BUILTIN2, LED_BUILTIN3}; // array when using SPRESENSE
static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN};
//static const int VIBR = 0;

// ------------------------- Debug variables --------------------------
boolean responsiveCalibration = false;
boolean printStrokes = true;
boolean printNormalizedValues_ = false;
String output_string[numInputs];

// ------------------ variables for interrupt timers ------------------
IntervalTimer pinMonitor; // reads pins every 1 ms
IntervalTimer masterClock; // 1 bar
IntervalTimer stateChecker; // runs functions and sets flags for timed functions etc
volatile int counts[numInputs];
volatile unsigned long lastPinActiveTime[numInputs];
volatile unsigned long firstPinActiveTime[numInputs];

// ------------------ timing and rhythm tracking ------------------
int countsCopy[numInputs];
int current_beat_pos; // always stores the current position in the beat

int tapInterval = 500; // 0.5 s per beat for 120 BPM
// int tapState = 1; // 0 = none; 1 = waiting for first hit; 2 = waiting for second hit


// ------------- sensitivity and instrument calibration -----------------
int noiseFloor[numInputs]; // typical values: Arduino (5V) = 512, SPRESENSE (3.3V) = 260
const int max_val = 1023;
//const int globalThreshold = 30; // typical values: 60-190 for toms (SPRESENSE, 3.3V)
const int threshold[] = {110, 90, 400, 400, 200, 60, 60, 60}; // minimum peak from noiseFloor to be counted
int min_counts_for_signature[] = {15, 20, 12, 40, 10, 1, 1, 1}; // characteristic instrument signatures; 0:snare, 1:hihat, 2:tom1, 3:Standtom
int globalDelayAfterStroke = 10; // 50 ms TODO: assess best timing for each instrument

/* --------------------------- MUSICAL PARAMETERS ---------------------- */

boolean rhythm_slot[numInputs][32];
int notes_list[] = {60, 61, 63, 65, 67, 68, 71}; // phrygian mode: {C, Des, Es, F, G, As, B}
int note_idx[numInputs]; // instrument-specific pointer for notes
int pinAction[] = {1, 1, 1, 1, 1, 1, 1, 1};
/* 0 = tapTempo
   1 = binary beat logger (print beat)
   2 = toggle rhythm_slot
*/

/* --------------------------------------------------------------------- */
/* ---------------------------------- SETUP ---------------------------- */
/* --------------------------------------------------------------------- */

void setup() {

  Serial.begin(115200);
  while (!Serial);
  // MIDI.begin(MIDI_CHANNEL_OMNI);

  //------------------------ initialize pins and arrays ------------------------
  for (int i = 0; i < numInputs; i++)
  {
    pinMode(leds[i], OUTPUT);
    counts[i] = 0;
  }
  //pinMode(VIBR, OUTPUT);
  // ---------------------------------------------------------------------------


  // ----------------------------- calculate noiseFloor ------------------------
  // calculates the average noise floor out of 400 samples from all inputs

  int led_idx = 0;
  for (int pinNum = 0; pinNum < numInputs; pinNum++)
  {
    Serial.print("calculating noiseFloor for pin ");
    Serial.print(pins[pinNum]);
    Serial.print(" ..waiting for stroke");
    if (responsiveCalibration)
    {
      while (analogRead(pins[pinNum]) < 500 + threshold[pinNum]); // calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
      Serial.print(" .");
      delay(1000); // should be long enough for drum not to oscillate anymore
    }

    int totalSamples = 0;
    boolean toggleState = false;
    for (int n = 0; n < 400; n++)
    {
      //Serial.print(".");
      if (n % 100 == 0)
      {
        Serial.print(" . ");
        digitalWrite(leds[pinNum], toggleState);
        toggleState = !toggleState;
      }
      totalSamples += analogRead(pins[pinNum]);
    }
    noiseFloor[pinNum] = totalSamples / 400;
    digitalWrite(leds[pinNum], LOW);
    led_idx++;
    Serial.print("noiseFloor = ");
    Serial.println(noiseFloor[pinNum]);
  }

  for (int i = 0; i < numInputs; i++) // turn LEDs off again
  {
    digitalWrite(leds[i], LOW);
    output_string[i] = "\t";
  }
  // ---------------------------------------------------------------------------

  // -------------------------- setup initial values ---------------------------
  // set start notes and rhythms
  note_idx[1] = 0; // C
  note_idx[2] = 3; // F
  note_idx[3] = 0;
  // ---------------------------------------------------------------------------


  // ------------------------------ start timer --------------------------------
  pinMonitor.begin(samplePins, 1000);  // sample pin every 1 millisecond
  masterClock.begin(masterClockTimer, tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
  //stateChecker.begin(checkStates, 10000); // 10 ms

  // ---------------------------------------------------------------------------
}


/* --------------------------------------------------------------------- */
/* -------------------------- TIMED INTERRUPTS ------------------------- */
/* --------------------------------------------------------------------- */

void samplePins()
{
  // ------------------------- read all pins -----------------------------------
  for (int pinNum = 0; pinNum < numInputs; pinNum++)
  {
    if (pinValue(pinNum) > threshold[pinNum])
    {
      if (counts[pinNum] < 1) firstPinActiveTime[pinNum] = millis();
      lastPinActiveTime[pinNum] = millis();
      counts[pinNum]++;
    }
  }
}
// -----------------------------------------------------------------------------

// ----------------------------- timer counter ---------------------------------
volatile unsigned long masterClockCount = 0; // 4*32 = 128 masterClockCount per cycle
volatile unsigned long beatCount = 0;
// volatile int bar_step; // 0-32
volatile int currentStep; // 0-32
int next_beatCount = 0;  // will be reset when timer restarts

void masterClockTimer()
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
  if ((masterClockCount % 4) >= next_beatCount - 2)
  {
    currentStep = next_beatCount;
    next_beatCount += 4;
  }
  // ---------------------------------------------------------------------------

}
/*void checkStates() // a timer to run several functions and set flags
{
  checkVibration(); // turns off vibration motor
}*/

/* --------------------------------------------------------------------- */
/* ------------------------------- LOOP -------------------------------- */
/* --------------------------------------------------------------------- */

void loop()
{

  // ------------------------- debug area -----------------------------
  printNormalizedValues(printNormalizedValues_);

  // --------------------- INCOMING SIGNALS FROM PIEZOS ---------------
  // (define what should happen when instruments are hit)
  for (int i = 0; i < numInputs; i++)
  {
    if (stroke_detected(i)) // evaluates pins for activity repeatedly
    {
      // ----------------------- perform pin action -------------------
      switch (pinAction[i])
      {
        case 0: // ------------------ tapTempo ------------------------

          getTapTempo();
          break;

        case 1: // --- monitor: just print what is being played. ------
          if (printStrokes)
          {
            //Serial.print(pins[i]);
            //Serial.print("\t");
            //Serial.print(countsCopy[i]);
            //Serial.print("\t");
            if (i == KICK) output_string[i] += "※\t"; // Kickdrum
            else if (i == STANDTOM1) output_string[i] += "O\t"; // Standtom
            else if (i == STANDTOM2) output_string[i] += "O\t"; // Standtom
            else if (i == HIHAT) output_string[i] += "x\t"; // Hi-Hat
            else if (i == TOM1) output_string[i] += "°\t"; // Tom 1
            else if (i == SNARE) output_string[i] += "᠅\t"; // Snaredrum
            else if (i == TOM2) output_string[i] += "o\t"; // Tom 2
            else if (i == RIDE) output_string[i] += "xx\t"; // Ride
            else if (i == CRASH1) output_string[i] += "X\t"; // Crash
            else if (i == CRASH2) output_string[i] += "XX\t"; // Crash
            // Serial.println("");
          }
          break;

        case 2: // ---------------- toggle beat slot ------------------
          // rhythm_slot[i][current_beat_pos] = !rhythm_slot[i][current_beat_pos];
          break;

        default:
          break;
      }

    }

  } // end main commands loop
  // ---------------------------------------------------------------------------

  // ------------------------------- TIMED ACTIONS --------------------
  // (automatically invoke rhythm-linked actions)
  static int last_beat_pos = 0;
  static boolean toggleLED = true;

  noInterrupts();
  current_beat_pos = beatCount % 32; // (beatCount increases infinitely)
  interrupts();

  if (current_beat_pos != last_beat_pos)
  {
    // ----------------------------- draw time log to console
    if (current_beat_pos % 4 == 0) toggleLED = !toggleLED;
    digitalWrite(LED_BUILTIN, toggleLED);
    for (int i = 0; i < numInputs; i++)
    {
      // if (rhythm_slot[pins[i]][current_beat_pos] == true)
      // {
      //   /* perform action, e.g. send MIDI note */
      // }
    }
    Serial.print(millis());
    Serial.print("\t");
    Serial.print(current_beat_pos);
    for (int i = 0; i < numInputs; i++)
    {
      Serial.print(output_string[i]);
      output_string[i] = "\t";
    }
    Serial.println("");

    // ---------------------------- vibrate on beat
    //vibrate(millis(), 50);
  }
  last_beat_pos = current_beat_pos;

}

/* --------------------------------------------------------------------- */
/* ----------------------------- FUNCTIONS ----------------------------- */
/* --------------------------------------------------------------------- */

int pinValue(int pinVal_pointer_in)
{
  return abs(noiseFloor[pinVal_pointer_in] - analogRead(pins[pinVal_pointer_in]));
}

boolean stroke_detected(int pinDect_pointer_in)
{
  static unsigned long lastPinActiveTimeCopy[numInputs];
  static unsigned long firstPinActiveTimeCopy[numInputs];
  static int lastValue[numInputs]; // for LED toggle
  static boolean toggleState = false; // for LED toggle

  noInterrupts();
  lastPinActiveTimeCopy[pinDect_pointer_in] = lastPinActiveTime[pinDect_pointer_in];
  firstPinActiveTimeCopy[pinDect_pointer_in] = firstPinActiveTime[pinDect_pointer_in];
  interrupts();

  if (millis() > lastPinActiveTimeCopy[pinDect_pointer_in] + globalDelayAfterStroke) // get counts only X ms after LAST hit
    // if (millis() > firstPinActiveTimeCopy[pinDect_pointer_in] + globalDelayAfterStroke) // get counts only X ms after FIRST hit ??
  {
    noInterrupts();
    countsCopy[pinDect_pointer_in] = counts[pinDect_pointer_in];
    counts[pinDect_pointer_in] = 0;
    interrupts();

    // ---------------------------- found significant count!
    if (countsCopy[pinDect_pointer_in] >= min_counts_for_signature[pinDect_pointer_in])
    {
      // LED blink:
      //if (countsCopy[pinDect_pointer_in] != lastValue[pinDect_pointer_in]) toggleState = !toggleState;
      //digitalWrite(LED_BUILTIN, toggleState);
      lastValue[pinDect_pointer_in] = countsCopy[pinDect_pointer_in];

      // countsCopy[pinDect_pointer_in] = 0;

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

void getTapTempo()
{
  static unsigned long timeSinceFirstTap = 0;
  static int tapState = 1;
  static int num_of_taps = 0;
  static int clock_sum = 0;

  switch (tapState) {
    //    case 0: // this is for activation of tap tempo listen
    //      // for (int i = 0; i < numInputs; i++)
    //      // {
    //      // }
    //      // puts("\n");
    //      tapState = 1;
    //      break;

    case 1:   // waiting for first hit
      if (millis() > timeSinceFirstTap + 10000) // reinitiate tap if not used for ten seconds
      {
        num_of_taps = 0;
        clock_sum = 0;
        // TODO: RHYTHM RESET???????????
        puts("TAP RESET!\n");
      }
      timeSinceFirstTap = millis(); // record time of first hit
      tapState = 2; // next: wait for second hit

      break;

    case 2:   // waiting for second hit

      if (timeSinceFirstTap < 2000) // only record tap if interval was not too long
      {
        num_of_taps++;
        clock_sum += timeSinceFirstTap;
        tapInterval = clock_sum / num_of_taps;
        // bpm = 60000 / tapInterval;
        tapState = 1;

        masterClock.begin(masterClockTimer, tapInterval * 1000 * 4 / 128); // taps are always understood as quarter notes
      }

      if (timeSinceFirstTap > 2000) // forget tap if time was too long
      {
        tapState = 1;
        // Serial.println("too long...");
      }
      // }
      break;
  }
}

// ---------------------------- vibration motor -------------------------
/*boolean vibrate_bool = false;
unsigned long vibration_begin_;
int vibration_duration_;

void vibrate(long vibration_begin, int vibration_duration)
{
  vibration_begin_ = vibration_begin;
  vibration_duration_ = vibration_duration;
  vibrate_bool = true;
}

void checkVibration() // is run in timer
{
  if (vibrate_bool)
  {
    if (millis() < vibration_begin_ + vibration_duration_)
    {
      digitalWrite(VIBR, HIGH);
    }
    else
    {
      digitalWrite(VIBR, LOW);
      vibrate_bool = false;
    }
  }
}
*/


/* ----------------------- DEBUG FUNCTIONS ---------------------------- */

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
        static int countsCopy[numInputs];
        //noInterrupts();
        //countsCopy[i] = counts[i];
        //interrupts();
        Serial.print(pins[i]);
        Serial.print("\t");
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
