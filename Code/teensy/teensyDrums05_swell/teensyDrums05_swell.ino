/*
  SWELL:
  repeats strokes for each drum on a midi instrument
  kinda like a delay effekt
  start with the effect only after a certain hit "density"
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
    3 = footswitch looper: records what is being played for one bar while footswitch is pressed and repeats it after release.
    4 = tapTempo: a standard tapTempo to change the overall pace. to be used on one instrument only.
    5 = "swell" effect: all instruments have a tap tempo that will retrigger MIDI notes accordingly
*/

/* --------------------------------------------------------------------- */
/* ------------------------------- GLOBAL ------------------------------ */
/* --------------------------------------------------------------------- */
#include <MIDI.h>

//MIDI_CREATE_DEFAULT_INSTANCE();
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); // for Serial-specific usage

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
boolean use_responsiveCalibration = false;
boolean printStrokes = true;
boolean printNormalizedValues_ = false;
boolean do_print_to_console = true;
boolean do_send_to_processing = false;
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

// ------------- sensitivity and instrument calibration -----------------
int calibration[numInputs][2]; // [instrument][0:threshold, 1:min_counts_for_signature], will be set in setup()
int noiseFloor[numInputs];
int globalDelayAfterStroke = 10; // 50 ms TODO: assess best timing for each instrument

// ----------------- MUSICAL AND PERFORMATIVE PARAMETERS --------------

boolean read_rhythm_slot[numInputs][8];
boolean set_rhythm_slot[numInputs][8];
//int notes_list[] = {60, 61, 39, 65, 67, 44, 71}; // phrygian mode: {C, Des, Es, F, G, As, B}
const int LOG_BEATS = 0;
const int HOLD_CC = 1; 
const int FOOTSWITCH_MODE = 1;

int notes_list[] = {60, 61, 73, 74, 67, 44, 71};
int cc_chan[] = {0, 0, 0, 25, 71, 50, 0, 0}; // needed in pinAction 5 and 6
/* channels on mKORG: 
 * 44=cutoff 
 * 71=resonance
 * 50=amplevel
 * 23=attack
 * 25=sustain
 * 26=release
 */
int pinAction[] = {1, 4, 1, 5, 5, 5, 1, 1}; // array to be changed within code loop.
int initialPinAction[numInputs];            // holds the pinAction array as defined above

/* pinActions:
    0 = tapTempo
    1 = binary beat logger (print beat)
    2 = toggle rhythm_slot
    3 = footswitch looper: records what is being played for one bar while footswitch is pressed and repeats it after release.
    4 = tapTempo: a standard tapTempo to change the overall pace. to be used on one instrument only.
    5 = "swell" effect: all instruments have a tap tempo that will retrigger MIDI notes accordingly
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
    for (int j = 0; j < 8; j++)
    {
      read_rhythm_slot[i][j] = false;
      set_rhythm_slot[i][j] = false;
    }
    initialPinAction[i] = pinAction[i];
  }
  swell_init();
  pinMode(VIBR, OUTPUT);
  pinMode(FOOTSWITCH, INPUT_PULLUP);

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

  calculateNoiseFloor();

  // setup initial values ---------------------------------------------

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

  Serial.println("-----------------------------------------------");
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
  Serial.println("-----------------------------------------------");

  //  start timer -----------------------------------------------------

  pinMonitor.begin(samplePins, 1000);                                // sample pin every 1 millisecond
  masterClock.begin(masterClockTimer, tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
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
  // 2020-09-07: this doesn't help and it's also not working...
  // if ((masterClockCount % 4) >= next_beatCount - 2)
  // {
  //   currentStep = next_beatCount;
  //   next_beatCount += 4;
  // }

  // prepare MIDI clock:
  sendMidiClock = (((masterClockCount % 4) / 3) % 4 == 0);

  // ---------------------------------------------------------------------------
}

/* --------------------------------------------------------------------- */
/* ------------------------------- LOOP -------------------------------- */
/* --------------------------------------------------------------------- */

void loop()
{
  static int eighthNoteCount = 0;
  static int last_eighth_count = 0;
  static unsigned long lastNotePlayed[numInputs];

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
      case 0:
        MIDI.sendNoteOn(notes_list[i], 127, 2);
        lastNotePlayed[i] = millis();
        break;

      case 1: // monitor: just print what is being played.
        if (printStrokes)
        {
          setInstrumentPrintString(i, pinAction[i]);
        }
        break;

      case 2: // toggle beat slot
        if (printStrokes)
          setInstrumentPrintString(i, pinAction[i]);
        read_rhythm_slot[i][eighthNoteCount] = !read_rhythm_slot[i][eighthNoteCount];
        break;

      case 3: // record what is being played and replay it later
        if (printStrokes)
          setInstrumentPrintString(i, pinAction[i]);
        set_rhythm_slot[i][eighthNoteCount] = true;
        break;

      case 4: // tapTempo
        getTapTempo();
        break;

      case 5: // "swell"
        setInstrumentPrintString(i, pinAction[i]);
        swell_rec(i);
        break;

      default:
        break;
      }
      
      // send instrument stroke to processing:
      send_to_processing(i);
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
  sendMidiCopy = sendMidiClock;      // MIDI flag for Clock to be sent
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
    sendMidiClock = false;
    interrupts();
  }

  // DO THINGS ONCE PER 32nd-STEP: ------------------------------------
  // ------------------------------------------------------------------
  if (current_beat_pos != last_beat_pos)
  {
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
      if (pinAction[i] == 3)
        read_rhythm_slot[i][eighthNoteCount] = set_rhythm_slot[i][eighthNoteCount];
    }

    // -------------------------- PIN ACTIONS: ------------------------
    for (int i = 0; i < numInputs; i++)
    {
      if (pinAction[i] == 5)
        swell_beat(i); // ...updates once a 32nd-beat-step

      else if (pinAction[i] == 2 || pinAction[i] == 3) // send MIDI notes (pinActions 2 and 3):
      {
        if (eighthNoteCount != last_eighth_count) // in 8th-interval
        {
          if (read_rhythm_slot[i][eighthNoteCount])
          {
            setInstrumentPrintString(i, 3);
            MIDI.sendNoteOn(notes_list[i], 127, 2);
          }
          else
            MIDI.sendNoteOff(notes_list[i], 127, 2);
        }
      }
    }
    // ----------------------------------------------------------------

    // ---------------------------- vibrate on beat:
    if (current_beat_pos % 8 == 0) // current_beat_pos holds 32 → %8 makes 4.
    {
      vibration_begin = millis();
      vibration_duration = 50;
      digitalWrite(VIBR, HIGH);
    }

    // ATTENTION: Sketch got very slow now after this... (31.08.2020)
    // ... really?
    // for (int i = 0; i < numInputs; i++)
    // if (pinAction[i] == 3)
    // set_rhythm_slot[i][eighthNoteCount] = false;

    // ----------------------------- draw play log to console
    print_to_console(String(millis()));
    print_to_console("\t");
    // Serial.print(eighthNoteCount + 1); // if you want to print 8th-steps only
    print_to_console(current_beat_pos);
    print_to_console("\t");
    /*Serial.print(current_beat_pos / 4);
        Serial.print("\t");
        Serial.print(eighthNoteCount);*/
    for (int i = 0; i < numInputs; i++)
    {
      print_to_console(output_string[i]);
      output_string[i] = "\t";
    }
    println_to_console("");

    //    if (current_beat_pos % 8 == 0)
    //    MIDI.sendNoteOn(57, 127, 2);
    //    else
    //    MIDI.sendNoteOff(57, 127, 2);

  } // --------------- end of (32nd-step) TIMED ACTIONS ---------------
  // ------------------------------------------------------------------

  last_beat_pos = current_beat_pos;
  last_eighth_count = eighthNoteCount;

  // check footswitch -------------------------------------------------
  checkFootSwitch();

  // turn off vibration -----------------------------------------------
  if (millis() > vibration_begin + vibration_duration)
    digitalWrite(VIBR, LOW);
  for (int i = 0; i < numInputs; i++)
    if (millis() > lastNotePlayed[i] + 200 && pinAction[i] != 5) // pinAction 5 turns notes off in swell_beat()
      MIDI.sendNoteOff(notes_list[i], 127, 2);
}
// --------------------------------------------------------------------
