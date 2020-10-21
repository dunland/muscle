#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h>
#include <Tsunami.h>
#include <MIDI.h>

//MIDI_CREATE_DEFAULT_INSTANCE();
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); // for Serial-specific usage

#define VIBR 0
#define FOOTSWITCH 2


class Globals
{
public:
  static const uint8_t numInputs = 7;
  static uint8_t leds[];
  static uint8_t pins[];

  static uint8_t pinAction[];
};

// ------------- sensitivity and instrument calibration -----------------
class Calibration
{
public:
  static int calibration[Globals::numInputs][2]; // [instrument][0:threshold, 1:min_counts_for_signature], will be set in setup()
  static int noiseFloor[Globals::numInputs];
  static int globalDelayAfterStroke;
};

// ------------------------- Debug variables --------------------------
class Debug
{
public:
  static boolean use_responsiveCalibration;
  static boolean printStrokes;
  static boolean printNormalizedValues_;
  static boolean do_print_to_console;
  static boolean do_send_to_processing;
  static String output_string[Globals::numInputs];
};

// ------------------------ Hardware Interfaces -----------------------
class Hardware
{
public:
  static Tsunami tsunami;

  static const int LOG_BEATS = 0;
  static const int HOLD_CC = 1;
  static const int RESET_TOPO = 2; // resets beat_topography (of all instruments)
  static const int FOOTSWITCH_MODE = 2;
  static boolean footswitch_is_pressed;
  static void footswitch_pressed();
  static void footswitch_released();
};

// ------------------------------ pins --------------------------------
class Core
{
  /////////////////////////// general pin reading ///////////////////////
  ///////////////////////////////////////////////////////////////////////
public:
  static int pinValue(int);
};

// ----------------- MUSICAL AND PERFORMATIVE PARAMETERS --------------
class Score
{
public:
  static boolean read_rhythm_slot[Globals::numInputs][8];
  static boolean set_rhythm_slot[Globals::numInputs][8];
  // static int beat_topography_8[Globals::numInputs][8]; // TODO: use structs instead!
  // static int beat_topography_16[Globals::numInputs][16];

  static int notes_list[];
  static int cc_chan[]; // needed in pinAction 5 and 6

  static int initialPinAction[Globals::numInputs]; // holds the pinAction array as defined above

  static int allocated_track[Globals::numInputs]; // tracks will be allocated in tsunami_beat_playback
  static int allocated_channels[];                // channels to send audio from tsunami to

  static float track_bpm[];

  static struct TOPOGRAPHY_16
  {
    int array[Globals::numInputs][16];
    int threshold;
    int average_smooth;
  } beat_topography_8, beat_topography_16, beat_regularity_16;
};

class Swell
{
public:
  static boolean lastPinAction[Globals::numInputs];
};

// ------------------ variables for interrupt timers ------------------
class Timing
{
public:
  static IntervalTimer pinMonitor;  // reads pins every 1 ms
  static IntervalTimer masterClock; // 1 bar
  static volatile int counts[Globals::numInputs];
  static volatile unsigned long lastPinActiveTime[Globals::numInputs];
  static volatile unsigned long firstPinActiveTime[Globals::numInputs];

  // ------------------ timing and rhythm tracking ------------------
  static int current_beat_pos; // always stores the current position in the beat

  static int tapInterval;
  static int current_BPM;

  // -------------------------- TIMERS --------------------------------
  static void samplePins();
  static void masterClockTimer();

  // ----------------------------- timer counter ---------------------------------
  static volatile unsigned long masterClockCount;
  static volatile unsigned long beatCount;
  static volatile int currentStep;
  static int next_beatCount;
  static volatile boolean sendMidiClock;
};

enum DrumType
{
  Snare,
  Hihat,
  Kick,
  Tom1,
  Tom2,
  Standtom1,
  Cowbell,
  Standtom2,
  Ride,
  Crash1,
  Crash2
};

String DrumtypeToHumanreadable(DrumType);

#endif
