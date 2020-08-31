/*
  logs drum playing, toggles binary instrument position on beat,
  plays MIDI-notes at those sites
   ------------------------------------
   August 2020
   by David Unland david[at]unland[dot]eu
   ------------------------------------
   ------------------------------------
  0. calibrate sensors
  1. set calibration[instrument][0/1] values. (in setup())
    0:threshold, 1:min num of threshold crossings
  2. set mode for instruments:
    pinActions:
    0 = tapTempo
    1 = binary beat logger (print beat)
    2 = toggle rhythm_slot
*/

/* --------------------------------------------------------------------- */
/* ------------------------------- GLOBAL ------------------------------ */
/* --------------------------------------------------------------------- */
#include <MIDI.h>

//MIDI_CREATE_DEFAULT_INSTANCE();
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); // for Serial-specific usage on SPRESENSE

// ----------------------------- pins ---------------------------------
static const uint8_t numInputs = 7;
static const uint8_t pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
#define SNARE 0
#define HIHAT 1
#define KICK 2
#define TOM1 3
#define TOM2 4
#define STANDTOM1 5
#define COWBELL 6
#define STANDTOM2 7
#define RIDE 8
#define CRASH1 9
#define CRASH2 10

char *names[10];

// static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN1, LED_BUILTIN2, LED_BUILTIN3}; // array when using SPRESENSE
static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN};
#define VIBR 0
#define FOOTSWITCH 2 

// ------------------------- Debug variables --------------------------
boolean responsiveCalibration = false;
boolean printStrokes = true;
boolean printNormalizedValues_ = false;
String output_string[numInputs];

// ------------------ variables for interrupt timers ------------------
IntervalTimer pinMonitor;   // reads pins every 1 ms
IntervalTimer masterClock;  // 1 bar
IntervalTimer stateChecker; // runs functions and sets flags for timed functions etc
volatile int counts[numInputs];
volatile unsigned long lastPinActiveTime[numInputs];
volatile unsigned long firstPinActiveTime[numInputs];

// ------------------ timing and rhythm tracking ------------------
int countsCopy[numInputs];
int current_beat_pos = 0; // always stores the current position in the beat

int tapInterval = 500; // 0.5 s per beat for 120 BPM
// int tapState = 1; // 0 = none; 1 = waiting for first hit; 2 = waiting for second hit

// ------------- sensitivity and instrument calibration -----------------

int calibration[numInputs][2]; // [instrument][0:threshold, 1:min_counts_for_signature], will be set in setup()
int noiseFloor[numInputs];
int globalDelayAfterStroke = 10; // 50 ms TODO: assess best timing for each instrument

/* --------------------------- MUSICAL PARAMETERS ---------------------- */

boolean rhythm_slot[numInputs][8];
int notes_list[] = {60, 61, 39, 65, 67, 44, 71}; // phrygian mode: {C, Des, Es, F, G, As, B}
int note_idx[numInputs];                         // instrument-specific pointer for notes
int pinAction[] = {2, 2, 2, 2, 2, 2, 0, 2};
/* pinActions:
   0 = tapTempo
   1 = binary beat logger (print beat)
   2 = toggle rhythm_slot
*/

/* --------------------------------------------------------------------- */
/* ---------------------------------- SETUP ---------------------------- */
/* --------------------------------------------------------------------- */

void setup()
{

  Serial.begin(115200);
  while (!Serial)
    ;
  MIDI.begin(MIDI_CHANNEL_OMNI);

  //------------------------ initialize pins and arrays ------------------------
  for (int i = 0; i < numInputs; i++)
  {
    pinMode(leds[i], OUTPUT);
    counts[i] = 0;
  }
  pinMode(VIBR, OUTPUT);

  // ---------------------------------------------------------------------------
  // instrument naming for human-readable console outputs
  names[SNARE] = "SNARE";
  names[HIHAT] = "HIHAT";
  names[KICK] = "KICK";
  names[TOM1] = "TOM1";
  names[TOM2] = "TOM2";
  names[STANDTOM1] = "STom1";
  names[STANDTOM2] = "STom2";
  names[CRASH1] = "CRSH1";
  names[CRASH2] = "CRSH2";
  names[RIDE] = "RIDE0";
  names[COWBELL] = "CBELL";

  // ----------------------------- calculate noiseFloor ------------------------
  // calculates the average noise floor out of 400 samples from all inputs

  int led_idx = 0;
  for (int pinNum = 0; pinNum < numInputs; pinNum++)
  {
    Serial.print("calculating noiseFloor for ");
    Serial.print(names[pinNum]);
    Serial.print(" ..waiting for stroke");
    if (responsiveCalibration)
    {
      while (analogRead(pins[pinNum]) < 700 + calibration[pinNum][0])
        ; // calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
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

  // set instrument calibration array
  // values as of 2020-08-27:
  calibration[SNARE][0] = 180;
  calibration[SNARE][1] = 12;
  calibration[HIHAT][0] = 60;
  calibration[HIHAT][1] = 20;
  calibration[KICK][0] = 100;
  calibration[KICK][1] = 16;
  calibration[TOM1][0] = 200;
  calibration[TOM1][1] = 20;
  calibration[STANDTOM1][0] = 70;
  calibration[STANDTOM1][1] = 12;
  calibration[TOM2][0] = 300;
  calibration[TOM2][1] = 18;
  calibration[COWBELL][0] = 80;
  calibration[COWBELL][1] = 15;

  /*
    if (!responsiveCalibration) // hard coded ?
    {
    noiseFloor[SNARE] = 470;
    noiseFloor[HIHAT] = 476;
    noiseFloor[KICK] = 475;
    noiseFloor[TOM1] = 477;
    noiseFloor[TOM2] = 471;
    noiseFloor[STom1] = 497;
    noiseFloor[CBELL] = 498;
    }
  */

  Serial.println();
  Serial.println("calibration values set as follows:");
  Serial.println("instr\tthrshld\tcrosses\tnoiseFloor");
  for (int i = 0; i < numInputs; i++)
  {
    Serial.print(names[i]);
    Serial.print("\t");
    for (int j = 0; j < 2; j++)
    {
      Serial.print(calibration[i][j]);
      Serial.print("\t");
    }
    Serial.println(noiseFloor[i]);
  }
  Serial.println();

  // set start notes and rhythms
  note_idx[1] = 0; // C
  note_idx[2] = 3; // F
  note_idx[3] = 0;

  // set all rhythm slots to false
  for (int i = 0; i < numInputs; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      rhythm_slot[i][j] = false;
    }
  }
  // ---------------------------------------------------------------------------

  // ------------------------------ start timer --------------------------------
  pinMonitor.begin(samplePins, 1000); // sample pin every 1 millisecond
  // higher precision:
  //masterClock.begin(masterClockTimer_higherPrecision, tapInterval * 1000 * 4 / 256); // 4 beats (1 bar) with 256 divisions in microseconds; initially 120 BPM
  // lower precision:
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
    if (pinValue(pinNum) > calibration[pinNum][0])
    {
      if (counts[pinNum] < 1)
        firstPinActiveTime[pinNum] = millis();
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
int next_beatCount = 0;   // will be reset when timer restarts
volatile boolean sendMidiClock = false;

void masterClockTimer_higherPrecision()
{
  /*
    timing              fraction          bar @ 120 BPM    bar @ 180 BPM
    1 bar             = 1               = 2 s       | 1.3 s
    1 beatCount       = 1/32 bar        = 62.5 ms   | 41.7 ms
    stroke precision  = 1/8 beatCount   = 7.53125 ms | 5.283 ms

    Snare drum, played with stick:
    |--------2 oscil---------|
    |     .-.
    |    /   \         .-.
    |   /     \       /   \       .-.     .-.     _   _
    +--/-------\-----/-----\-----/---\---/---\---/-\-/-\/\/---
    | /         \   /       \   /     '-'     '-'
    |/           '-'         '-'
    |--------- 10 ms --------|

  */

  masterClockCount++; // will rise infinitely

  // ------------ 1/32 increase in 4 full precisionCounts -----------
  if (masterClockCount % 8 == 0)
  {
    beatCount++; // will rise infinitely
  }

  // evaluate current position of beat in bar for stroke precision
  if ((masterClockCount % 8) >= next_beatCount - 4)
  {
    currentStep = next_beatCount;
    next_beatCount += 8;
  }

  // prepare MIDI clock:
  //---|---↓32nd-notes------|3/4↓ 32nd note|
  if (((masterClockCount % 4) * 3) % 4 == 0)
    sendMidiClock = true;

  // ---------------------------------------------------------------------------
}

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

/* --------------------------------------------------------------------- */
/* ------------------------------- LOOP -------------------------------- */
/* --------------------------------------------------------------------- */

void loop()
{
  static int eighthNoteCount = 0;
  static int last_eighth_count = 0;
  static boolean set_rhythm_slot[numInputs][8];

  // ------------------------- DEBUG AREA -----------------------------
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
      case 0: // tapTempo

        getTapTempo();
        break;

      case 1: // monitor: just print what is being played.
        if (printStrokes)
        {
          if (!rhythm_slot[i][eighthNoteCount])
            setInstrumentPrintString(i);
        }
        break;

      case 2: // toggle beat slot
        if (printStrokes)
          setInstrumentPrintString(i);
        read_rhythm_slot[i][eighthNoteCount] = !read_rhythm_slot[i][eighthNoteCount];
        break;

      case 3: // record what is being played for one full bar and set it later
        if (printStrokes)
          setInstrumentPrintString(i);
        set_rhythm_slot[i][eighthNoteCount] = true;
        break;

      default:
        break;
      }
    }
  } // end main commands loop -----------------------------------------

  // ------------------------------- TIMED ACTIONS --------------------
  // (automatically invoke rhythm-linked actions)
  static int last_beat_pos = 0;
  static boolean toggleLED = true;
  static boolean sendMidiCopy = false;

  static unsigned long vibration_begin = 0;
  static int vibration_duration = 0;

  noInterrupts();
  current_beat_pos = beatCount % 32; // (beatCount increases infinitely)
  sendMidiCopy = sendMidiClock;
  interrupts();

  // ---------------------------- send MIDI-Clock on beat
  /* MIDI Clock events are sent at a rate of 24 pulses per quarter note
     one tap beat equals one quarter note
     only one midi clock signal should be send per 24th quarter note
    */
  if (sendMidiCopy)
  {
    Serial2.write(0xF8); // MIDI-clock has to be sent 24 times per beat
    noInterrupts();
    sendMidiClock = false;
    interrups();
  }

  // ------------------------------ FULL NOTES -----------------------
  if (current_beat_pos % 32 == 0)
  {
    for (int i = 0; i < numInputs; i++)
    {
      set_rhythm_slot[i] = false;
    }
  }

  if (current_beat_pos != last_beat_pos)
  {
    // ------------------------------ 8th NOTES -----------------------
    // increase 8th note counter:
    if (current_beat_pos % 4 == 0)
    {
      eighthNoteCount = (eighthNoteCount + 1) % 8;
      toggleLED = !toggleLED;
    }
    digitalWrite(LED_BUILTIN, toggleLED);

    // set rhythm slots to play MIDI notes (pinMode 3):
    for (int i = 0; i < numInputs; i++)
    {
      if (pinMode[i] == 3)
        read_rhythm_slot[i][eighthNoteCount] = set_rhythm_slot[i][eighthNoteCount];
    }

    // set print String and send MIDI notes ---------------------------
    if (eighthNoteCount != last_eighth_count)
    {
      for (int i = 0; i < numInputs; i++)
      {
        if (read_rhythm_slot[i][eighthNoteCount])
        {
          setInstrumentPrintString(i);
          MIDI.sendNoteOn(notes_list[i], 127, 2);
        }
        else
          MIDI.sendNoteOff(notes_list[i], 127, 2);
      }

      // ----------------------------- draw play log to console
      Serial.print(millis());
      Serial.print("\t");
      Serial.print(eighthNoteCount + 1);
      //Serial.print(current_beat_pos);
      Serial.print("\t");
      /*Serial.print(current_beat_pos / 4);
      Serial.print("\t");
      Serial.print(eighthNoteCount);*/
      for (int i = 0; i < numInputs; i++)
      {
        Serial.print(output_string[i]);
        output_string[i] = "\t";
      }
      Serial.println("");
    }

    // ---------------------------- vibrate on beat
    if (current_beat_pos % 8 == 0) // current_beat_pos holds 32 → %8 makes 4.
    {
      vibration_begin = millis();
      vibration_duration = 50;
      digitalWrite(VIBR, HIGH);
    }
  }
  last_beat_pos = current_beat_pos;
  last_eighth_count = eighthNoteCount;

  // ------------------------ end of TIMED ACTIONS ------------------

  // check footswitch -------------------------------------------------
  static int switch_state = LOW;
  static int last_switch_state = LOW;
  static unsigned long last_switch_toggle = 0;
  static boolean lastPinMode[numInputs];

  switch_state = digitalRead(FOOTSWITCH);
  if (switch_state != last_switch_state && millis() > last_switch_toggle + 20)
  {
    if (switch_state == HIGH)
    {
      // set pinMode of all instruments to 3 (record what is being played)
      for (int i = 0; i < numInputs; i++)
      {
        lastPinMode[i] = pinMode[i];
        pinMode[i] = 3; // TODO: not for Cowbell?
      }
    }
    else 
    {
      for (int i = 0; i < numInputs; i++)
      {
        pinMode[i] = lastPinMode[i];
      }
    }
    last_switch_toggle = millis();
  }

  // turn off vibration -----------------------------------------------
  if (millis() > vibration_begin + vibration_duration)
    digitalWrite(VIBR, LOW);
}

/* --------------------------------------------------------------------- */
/* ----------------------------- FUNCTIONS ----------------------------- */
/* --------------------------------------------------------------------- */

// ------------------------ general pin reading -----------------------
int pinValue(int pinVal_pointer_in)
{
  return abs(noiseFloor[pinVal_pointer_in] - analogRead(pins[pinVal_pointer_in]));
}

boolean stroke_detected(int pinDect_pointer_in)
{
  static unsigned long lastPinActiveTimeCopy[numInputs];
  static unsigned long firstPinActiveTimeCopy[numInputs];
  static int lastValue[numInputs];    // for LED toggle
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
    if (countsCopy[pinDect_pointer_in] >= calibration[pinDect_pointer_in][1])
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

// ------------------------------ TAP TEMPO ---------------------------
void getTapTempo()
{
  static unsigned long timeSinceFirstTap = 0;
  static int tapState = 1;
  static int num_of_taps = 0;
  static int clock_sum = 0;

  switch (tapState)
  {
    //    case 0: // this is for activation of tap tempo listen
    //      // for (int i = 0; i < numInputs; i++)
    //      // {
    //      // }
    //      // Serial.print("\n");
    //      tapState = 1;
    //      break;

  case 1:                                     // waiting for first hit
    if (millis() > timeSinceFirstTap + 10000) // reinitiate tap if not used for ten seconds
    {
      num_of_taps = 0;
      clock_sum = 0;
      Serial.println("-----------TAP RESET!-----------\n");
    }
    timeSinceFirstTap = millis(); // record time of first hit
    tapState = 2;                 // next: wait for second hit

    break;

  case 2: // waiting for second hit

    if (millis() < timeSinceFirstTap + 2000) // only record tap if interval was not too long
    {
      num_of_taps++;
      clock_sum += millis() - timeSinceFirstTap;
      tapInterval = clock_sum / num_of_taps;
      Serial.print("new tap Tempo is ");
      Serial.print(60000 / tapInterval);
      Serial.print(" bpm (");
      Serial.print(tapInterval);
      Serial.println(" ms interval)");

      // bpm = 60000 / tapInterval;
      tapState = 1;

      masterClock.begin(masterClockTimer, tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
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

// ---------- print instrument at String position ---------------------
void setInstrumentPrintString(int incoming_i)
{
  if (incoming_i == KICK)
    output_string[incoming_i] = "■\t"; // Kickdrum
  else if (incoming_i == COWBELL)
    output_string[incoming_i] = "▲\t"; // Crash
  else if (incoming_i == STANDTOM1)
    output_string[incoming_i] = "□\t"; // Standtom
  else if (incoming_i == STANDTOM2)
    output_string[incoming_i] = "O\t"; // Standtom
  else if (incoming_i == HIHAT)
    output_string[incoming_i] = "x\t"; // Hi-Hat
  else if (incoming_i == TOM1)
    output_string[incoming_i] = "°\t"; // Tom 1
  else if (incoming_i == SNARE)
    output_string[incoming_i] = "※\t"; // Snaredrum
  else if (incoming_i == TOM2)
    output_string[incoming_i] = "o\t"; // Tom 2
  else if (incoming_i == RIDE)
    output_string[incoming_i] = "xx\t"; // Ride
  else if (incoming_i == CRASH1)
    output_string[incoming_i] = "-X-\t"; // Crash
  else if (incoming_i == CRASH2)
    output_string[incoming_i] = "-XX-\t"; // Crash
}

// --------------------------------------------------------------------

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
