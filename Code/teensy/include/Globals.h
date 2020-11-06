#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Tsunami.h> // in cpp
// #include <MIDI.h> // in cpp
#include <vector>
#include <Instruments.h>

//MIDI_CREATE_DEFAULT_INSTANCE();
// MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); // for Serial-specific usage

#define VIBR 0
#define FOOTSWITCH 2

class Instrument;

// instrument naming for human-readable console outputs:
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

enum EffectsType
{
  PlayMidi = 0,
  Monitor = 1,
  ToggleRhythmSlot = 2,
  FootSwitchLooper = 3,
  TapTempo = 4,
  Swell = 5,
  TsunamiLink = 6,
  CymbalSwell = 7,
  TopographyLog = 8,
  PlayMidi_rawPin = 9, // instead of stroke detection, MIDI notes are sent directly when sensitivity threshold is crossed. may sound nice on cymbals..
  CC_Effect_rawPin = 10
};

enum CC_Type // channels on mKORG:
{
   Cutoff = 44,
   Resonance = 71,
   Amplevel = 50,
   Attack = 23,
   Sustain = 25,
   Release = 26,
   DelayTime = 51,
   DelayDepth = 94
};

struct TOPOGRAPHY
{
  std::vector<int> a_8 = {0, 0, 0, 0, 0, 0, 0, 0};                          // size-8 array for comparison with 8-bit-length sound files
  std::vector<int> a_16 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // size-16 array for abstractions like beat regularity etc
  std::vector<int> a_16_prior = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  int snr_thresh = 3;         // threshold for signal-to-noise-ratio to be smoothened
  int activation_thresh = 10; // threshold in average_smooth to activate next action
  int average_smooth;
  int regular_sum = 0;
  String tag; // very short name for topography. also to be sent via Serial to processing

  boolean flag_entry_dismissed = false; // indicates that an entry has been dropped due to too high topography difference

  boolean flag_empty_increased = false;    // indicates that an empty slot has repeatedly NOT been played → increase
  boolean flag_occupied_increased = false; // indicates that an occupied slot has repeatedly been played → increase
  boolean flag_empty_played = false;       // indicates that an empty slot WAS played → decrease
  boolean flag_occupied_missed = false;    // indicates that an occupied slot has NOT been played → decrease
  // boolean change_expected[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
};

class Globals
{
public:
  static const uint8_t numInputs = 7;
  static std::vector<Instrument*> instruments;
  static std::vector<int> pins; // stores contact piezo pin of each instrument
  static std::vector<int> leds; // stores leds for each instrument (if differing)
  static int score_state;

  // ------------------------- Debug variables --------------------------
  static boolean do_print_to_console;
  static boolean do_send_to_processing;
  static boolean printStrokes;
  // static String output_string[numInputs];
  static boolean use_responsiveCalibration;

  static int tapInterval;
  static int current_BPM;

  // ------------------ variables for interrupt timers ------------------

  static IntervalTimer masterClock; // 1 bar

  static volatile unsigned long masterClockCount; // 4*32 = 128 masterClockCount per cycle
  static volatile unsigned long beatCount;        // static volatile int currentStep; // 0-32
  static int next_beatCount;                      // will be reset when timer restarts
  static volatile boolean sendMidiClock;

  static void masterClockTimer();

  static int current_beat_pos;     // always stores the current position in the beat
  static int current_eighth_count; // overflows at current_beat_pos % 8
  static int current_16th_count;   // overflows at current_beat_pos % 2
  static int last_eighth_count;    // stores last eightNoteCount for comparison
  static int last_16th_count;      // stores last eightNoteCount for comparison

  // ----------------------------- Hardware ---------------------------

  static Tsunami tsunami;
  static boolean footswitch_is_pressed;

  // hard-coded list of BPMs of tracks stored on Tsunami's SD card.
  // TODO: somehow make BPM accessible from file title
  static float track_bpm[256];

  // ----------------------------- Auxiliary --------------------------

  static String DrumtypeToHumanreadable(DrumType type)
  {
    switch (type)
    {
    case Snare:
      return "Snare";
    case Hihat:
      return "Hihat";
    case Kick:
      return "Kick";
    case Tom1:
      return "Tom1";
    case Tom2:
      return "Tom2";
    case Standtom1:
      return "S_Tom1";
    case Standtom2:
      return "S_Tom2";
    case Ride:
      return "Ride";
    case Crash1:
      return "Crash1";
    case Crash2:
      return "Crash2";
    case Cowbell:
      return "Cowbell";
    }
    return "";
  }

  static void setInstrumentPrintString(Instrument *instrument);

  // TOPOGRAPHIES: ------------------------------------------------------
  static void smoothen_dataArray(TOPOGRAPHY *topography); // there is a double of this in Instruments to perform instrument-specific operations
  static void derive_topography(TOPOGRAPHY *original, TOPOGRAPHY *abstraction);

  // DEBUG FUNCTIONS: ---------------------------------------------------
  // print the play log to Arduino console:
  static void print_to_console(String message_to_print);
  static void print_to_console(int int_to_print);
  static void print_to_console(float float_to_print);

  static void println_to_console(String message_to_print);
  static void println_to_console(int int_to_print);
  static void println_to_console(float float_to_print);

  // or send stuff to processing:
  static void send_to_processing(int message_to_send);
  static void send_to_processing(char message_to_send);

  static void printTopoArray(TOPOGRAPHY *topography);
  static void topo_array_to_processing(TOPOGRAPHY *topo);
  // --------------------------------------------------------------------
};

#endif
