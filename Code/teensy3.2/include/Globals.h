#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
// #include <Tsunami.h> // in cpp
#include <vector>

class Song;

enum MachineState
{
  Running,
  Calibration,
  NanoKontrol_Test
};

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
  TopographyMidiEffect = 8,
  Change_CC = 9,
  Random_CC_Effect = 10,
  MainNoteIteration = 11,
  Reflex_and_PlayMidi = 12,
  TapTempoRange
};

enum CC_Type
{
  CC_None = -1,

  // mikroKORG CC channels:
  mKORG_Arpeggio_onOff = 2, // 0-63: off, 64-127: on
  mKORG_Osc2_semitone = 18,
  mKORG_Osc2_tune = 19,
  mKORG_Mix_Level_1 = 20,
  mKORG_Mix_Level_2 = 21,
  mKORG_Patch_1_Depth = 28,
  mKORG_Patch_3_Depth = 30,
  mKORG_Cutoff = 44,
  mKORG_LFO1_Rate = 46,
  mKORG_LFO2_Rate = 76,
  mKORG_Resonance = 71,
  mKORG_Amplevel = 50,
  mKORG_Attack = 23,
  mKORG_Sustain = 25,
  mKORG_Release = 26,
  mKORG_DelayTime = 51,
  mKORG_Filter_Type = 83,
  mKORG_DelayDepth = 94,
  mKORG_TimbreSelect = 95,

  // DD-200 CC channels:
  dd200_DelayTime = 17,
  dd200_DelayLevel = 19,
  dd200_mod = 21,
  dd200_param = 22,
  dd200_OnOff = 27,
  dd200_DelayDepth = 94,

  // KP3 CC Channels:
  KP3_touch_pad_x = 12,
  KP3_touch_pad_y = 93,
  KP3_touch_pad_on_off = 92,
  KP3_Level_Slider = 93,
  KP3_FX_Depth = 94,
  KP3_Hold = 95,
  KP3_Sample_A = 36,
  KP3_Sample_B = 37,
  KP3_Sample_C = 38,
  KP3_Sample_D = 39

};

class TOPOGRAPHY
{
public:
  String tag; // very short name for topography. also to be sent via Serial to processing

  std::vector<int> a_8 = {0, 0, 0, 0, 0, 0, 0, 0};                          // size-8 array for comparison with 8-bit-length sound files
  std::vector<int> a_16 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // size-16 array for abstractions like beat regularity etc
  std::vector<int> a_16_prior = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  int snr_thresh = 3;         // threshold for signal-to-noise-ratio to be smoothened
  int activation_thresh = 10; // threshold in average_smooth to activate next action
  int average_smooth = 0;
  int regular_sum = 0;

  bool ready(); // holds whether average_smooth has reached activation_thresh

  boolean flag_entry_dismissed = false; // indicates that an entry has been dropped due to too high topography difference

  // -------------------------- regularity ----------------------------
  int regularity = 0; // the regularity of a played instrument boiled down to one value

  bool flag_empty_increased[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};    // indicates that an empty slot has repeatedly NOT been played → increase
  bool flag_occupied_increased[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false}; // indicates that an occupied slot has repeatedly been played → increase
  bool flag_empty_played[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};       // indicates that an empty slot WAS played → decrease
  bool flag_occupied_missed[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};    // indicates that an occupied slot has NOT been played → decrease

  // ---------------------------- functions ---------------------------
  void reset();
  void add(TOPOGRAPHY *to_add);
  void smoothen_dataArray(); // there is a double of this in Instruments to perform instrument-specific operations
  void derive_from(TOPOGRAPHY *original);
};

///////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBALS ////////////////////////////
///////////////////////////////////////////////////////////////////////
class Globals
{
public:
  // ------------------------- Debug variables --------------------------
  static boolean do_print_to_console;
  static boolean do_send_to_processing;
  static boolean printStrokes;
  static boolean use_responsiveCalibration;
  static boolean do_print_beat_sum; // prints Score::beat_sum topography array
  static boolean do_print_JSON;     // determines whether to use USB Serial communication for monitoring via processing/console or not

  static int tapInterval;
  static int current_BPM;
  static bool bSendMidiClock; // whether or not to send Midi Clock signal

  // ------------------ Machine State ------------------
  static MachineState machine_state;

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
  static int last_eighth_count;    // stores last eighthNoteCount for comparison
  static int last_16th_count;      // stores last 16thNoteCount for comparison

  // ----------------------------- Hardware ---------------------------

  // static Tsunami tsunami; // TODO: make this be part of Hardware
  static boolean footswitch_is_pressed;

  // hard-coded list of BPMs of tracks stored on Tsunami's SD card.
  // TODO: somehow make BPM accessible from file title
  static float track_bpm[256];

  // ------------------------------- Score ----------------------------

  static std::vector<Song *> songlist; // all relevant scores
  static int active_score_pointer; // points at active score of scores_list
  static Song *active_song;

  // ----------------------------- Auxiliary --------------------------

  static String DrumtypeToHumanreadable(DrumType type);

  static String EffectstypeToHumanReadable(EffectsType type);

  static String CCTypeToHumanReadable(CC_Type type);

  static CC_Type int_to_cc_type(int);

  // DEBUG FUNCTIONS: ---------------------------------------------------
  // print the play log to Serial monitor:
  static void print_to_console(String message_to_print);
  static void print_to_console(int int_to_print);
  static void print_to_console(float float_to_print);

  static void println_to_console(String message_to_print);
  static void println_to_console(int int_to_print);
  static void println_to_console(float float_to_print);

  static void printTopoArray(TOPOGRAPHY *topography);
  // --------------------------------------------------------------------
};

#endif
