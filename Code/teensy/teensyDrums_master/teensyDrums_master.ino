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
#include <MIDI.h>
#include <Tsunami.h>

Tsunami tsunami;

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
int current_BPM = 120;

// ------------- sensitivity and instrument calibration -----------------
int calibration[numInputs][2]; // [instrument][0:threshold, 1:min_counts_for_signature], will be set in setup()
int noiseFloor[numInputs];
int globalDelayAfterStroke = 10; // 50 ms TODO: assess best timing for each instrument

// ----------------- MUSICAL AND PERFORMATIVE PARAMETERS --------------

boolean read_rhythm_slot[numInputs][8];
boolean set_rhythm_slot[numInputs][8];
int beat_topography_8[numInputs][8];
int beat_topography_16[numInputs][16];

const int LOG_BEATS = 0;
const int HOLD_CC = 1;
const int RESET_TOPO = 2; // resets beat_topography (of all instruments)
const int FOOTSWITCH_MODE = 2;

int notes_list[] = {60, 61, 45, 74, 67, 44, 71};
int cc_chan[] = {50, 0, 0, 25, 71, 50, 0, 0}; // needed in pinAction 5 and 6
/* channels on mKORG:
   44=cutoff
   71=resonance
   50=amplevel
   23=attack
   25=sustain
   26=release
*/
int pinAction[] = {8, 4, 8, 1, 1, 1, 1, 1};       // array to be changed within code loop.
int initialPinAction[numInputs];                  // holds the pinAction array as defined above
int allocated_track[numInputs];                   // tracks will be allocated in tsunami_beat_playback
int allocated_channels[] = {0, 0, 0, 0, 0, 0, 0}; // channels to send audio from tsunami to

// hard-coded list of BPMs of tracks stored on Tsunami's SD card.
// TODO: somehow make BPM accessible from file title
float track_bpm[256] =
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
  1, 1, 1, 1, 100, 60
};

/* --------------------------------------------------------------------- */
/* ---------------------------------- SETUP ---------------------------- */
/* --------------------------------------------------------------------- */

void setup()
{

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
    counts[i] = 0;
    for (int j = 0; j < 8; j++)
    {
      read_rhythm_slot[i][j] = false;
      set_rhythm_slot[i][j] = false;
      beat_topography_8[i][j] = 0;
      beat_topography_16[i][j] = 0;
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

  // calculate noise floor:
  calculateNoiseFloor();

  // setup initial values ---------------------------------------------

  // set instrument calibration array
  // values as of 2020-08-27:
  calibration[SNARE][0] = 180;
  calibration[SNARE][1] = 12;
  calibration[HIHAT][0] = 70; // 60
  calibration[HIHAT][1] = 30; // 20
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

  // Debug:
  // tsunami.trackPlayPoly(1, 0, true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
  // tracknum, channel
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
  static int current_eighth_count = 0; // overflows at current_beat_pos % 8
  static int current_16th_count = 0;   // overflows at current_beat_pos % 2
  static int last_eighth_count = 0;    // stores last eightNoteCount for comparison
  static int last_16th_count = 0;    // stores last eightNoteCount for comparison
  static unsigned long lastNotePlayed[numInputs];

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
          read_rhythm_slot[i][current_eighth_count] = !read_rhythm_slot[i][current_eighth_count];
          break;

        case 3: // record what is being played and replay it later
          if (printStrokes)
            setInstrumentPrintString(i, pinAction[i]);
          set_rhythm_slot[i][current_eighth_count] = true;
          break;

        case 4: // tapTempo
          getTapTempo();
          break;

        case 5: // "swell"
          setInstrumentPrintString(i, pinAction[i]);
          swell_rec(i);
          break;

        case 6: // Tsunami beat-linked pattern
          setInstrumentPrintString(i, 1);
          beat_topography_8[i][current_eighth_count]++;
          break;

        case 7: // swell-effect for loudness on field recordings (use on cymbals e.g.)
          // TODO: UNTESTED! (2020-10-09)
          setInstrumentPrintString(i, pinAction[i]);
          swell_rec(i);
          break;

        case 8: // create beat-topography in 16-th
          setInstrumentPrintString(i, 1);
          beat_topography_16[i][current_16th_count]++;
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
      current_eighth_count = (current_eighth_count + 1) % 8;
      toggleLED = !toggleLED;
    }
    digitalWrite(LED_BUILTIN, toggleLED);

    // increase 16th note counter:
    if (current_beat_pos % 2 == 0)
    {
      current_16th_count = (current_16th_count + 1) % 16;
    }

    // -------------------------- PIN ACTIONS: ------------------------
    // ----------------------------------------------------------------
    for (int i = 0; i < numInputs; i++)
    {
      // --------------------------- SWELL: ---------------------------
      if (pinAction[i] == 5 || pinAction[i] == 7)
        swell_perform(i, pinAction[i]); // ...updates once a 32nd-beat-step

      else if (pinAction[i] == 3) // set rhythm slots to play MIDI notes:
        read_rhythm_slot[i][current_eighth_count] = set_rhythm_slot[i][current_eighth_count];

      // ----------- (pinActions 2 and 3): send MIDI notes ------------
      else if (pinAction[i] == 2 || pinAction[i] == 3)
      {
        if (current_eighth_count != last_eighth_count) // in 8th-interval
        {
          if (read_rhythm_slot[i][current_eighth_count])
          {
            setInstrumentPrintString(i, 3);
            MIDI.sendNoteOn(notes_list[i], 127, 2);
          }
          else
            MIDI.sendNoteOff(notes_list[i], 127, 2);
        }
      }

      // ----------------------- TSUNAMI PLAYBACK: --------------------
      else if (pinAction[i] == 6)
      {
        tsunami_beat_playback(i, current_beat_pos);
      }

      // ---------- MIDI playback according to beat_topography --------
      else if (pinAction[i] == 8)
      {
        if (current_16th_count != last_16th_count) // do this only once per 16th step
        {

          // create overall volume topography of all instrument layers:
          int total_vol[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
          for (int idx = 0; idx < 16; idx++)
          {
            for (int instr = 0; instr < numInputs; instr++)
            {
              if (pinAction[instr] == 8)
                total_vol[idx] += beat_topography_16[instr][idx];
            }
          }
          // print volume layer:
          Serial.print("vol:\t[");
          for (int j = 0; j < 16; j++)
          {
            Serial.print(total_vol[j]);
            if (j < 15)
              Serial.print(",");
          }
          Serial.println("]");

          int vol = min(total_vol[current_16th_count] * 25, 255);

          // change volume and play MIDI:
          if (beat_topography_16[i][current_16th_count] > 0)
          {
            MIDI.sendControlChange(cc_chan[i], vol, 2);
            MIDI.sendNoteOn(notes_list[i], 127, 2);
          }
          else
          {
            MIDI.sendNoteOff(notes_list[i], 127, 2);
          }


          // Debug:
          Serial.print(names[i]);
          Serial.print(":\t[");
          for (int j = 0; j < 16; j++)
          {
            Serial.print(beat_topography_16[i][j]);
            if (j < 15)
              Serial.print(",");
          }
          Serial.println("]");
        }
      }
    } // end pin Actions
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
    // Serial.print(current_eighth_count + 1); // if you want to print 8th-steps only
    print_to_console(current_beat_pos);
    print_to_console("\t");
    // Serial.print(current_beat_pos / 4);
    // Serial.print("\t");
    // Serial.print(current_eighth_count);
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

  last_beat_pos = current_beat_pos;
  last_eighth_count = current_eighth_count;
  last_16th_count = current_16th_count;

  // check footswitch -------------------------------------------------
  checkFootSwitch();

  // turn off vibration and MIDI notes --------------------------------
  if (millis() > vibration_begin + vibration_duration)
    digitalWrite(VIBR, LOW);

  for (int i = 0; i < numInputs; i++)
    if (millis() > lastNotePlayed[i] + 200 && pinAction[i] != 5) // pinAction 5 turns notes off in swell_beat()
      MIDI.sendNoteOff(notes_list[i], 127, 2);
}
// --------------------------------------------------------------------
