#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Tsunami.h> // in cpp
// #include <MIDI.h> // in cpp
#include <vector>

//MIDI_CREATE_DEFAULT_INSTANCE();
// MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); // for Serial-specific usage

#define VIBR 0
#define FOOTSWITCH 2

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
  TopographyLog = 8
};

class Globals
{
public:
  static const uint8_t numInputs = 7;
  static std::vector<int> pins; // stores contact piezo pin of each instrument
  static std::vector<int> leds; // stores leds for each instrument (if differing)

  // ------------------------- Debug variables --------------------------
  static boolean printNormalizedValues_;
  static boolean do_print_to_console;
  static boolean do_send_to_processing;
  static boolean printStrokes;
  static String output_string[numInputs];
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

  static void setInstrumentPrintString(DrumType drum, EffectsType effect);

  // DEBUG FUNCTIONS: ---------------------------------------------------
  // print the play log to Arduino console:
  static void print_to_console(String message_to_print);

  static void println_to_console(String message_to_print);

  // or send stuff to processing:
  static void send_to_processing(int message_to_send);
  // --------------------------------------------------------------------
};

struct TOPOGRAPHY
{
  int a[16];
  int a_8[8];   // 8-bit array for comparison with 8-bit-length sound files
  int a_16[16]; // 16-bit array for abstractions like beat regularity etc
  int threshold = 3;
  int average_smooth;
  int regular_sum = 0;
};

// TODO: may be obsolete..
// ---------------------- smoothen array of all sizes -----------------

// void smoothen_dataArray(int input_array[], int threshold_to_omit_entry = 3)
// {
//   int len = *(&input_array + 1) - input_array;
//   int entries = 0;
//   int squared_sum = 0;
//   int regular_sum = 0;

//   // count entries and create squared sum:
//   for (int j = 0; j < len; j++)
//   {
//     if (input_array[j] > 0)
//     {
//       entries++;
//       squared_sum += input_array[j] * input_array[j];
//       regular_sum += input_array[j];
//     }
//   }

//   regular_sum = regular_sum / entries;

//   // calculate site-specific (squared) fractions of total:
//   float squared_frac[len];
//   for (int j = 0; j < len; j++)
//     squared_frac[j] =
//         float(input_array[j]) / float(squared_sum);

//   // get highest frac:
//   float highest_squared_frac = 0;
//   for (int j = 0; j < len; j++)
//     highest_squared_frac = (squared_frac[j] > highest_squared_frac) ? squared_frac[j] : highest_squared_frac;

//   // get "topography height":
//   // divide highest with other entries and reset entries if ratio > threshold:
//   for (int j = 0; j < len; j++)
//     if (squared_frac[j] > 0)
//       if (highest_squared_frac / squared_frac[j] > 3 || squared_frac[j] / highest_squared_frac > threshold_to_omit_entry)
//       {
//         input_array[j] = 0;
//         entries -= 1;
//       }
// }

#endif