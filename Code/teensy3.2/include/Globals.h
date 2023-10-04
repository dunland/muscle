#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
// #include <Song.h>
// #include <Tsunami.h> // in cpp
#include <vector>

class Song;

enum MachineState
{
  Running,
  Calibrating,
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
  Standtom,
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

///////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBALS ////////////////////////////
///////////////////////////////////////////////////////////////////////
class Globals
{
public:

  static int tapInterval;
  static int current_BPM;
  static bool bSendMidiClock; // whether or not to send Midi Clock signal
  static bool bUsingSDCard;

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
  static int active_song_pointer; // points at active score of scores_list
  static Song *active_song;
  static Song *get_song(String songName);

  // ----------------------------- Auxiliary --------------------------

  static String DrumtypeToHumanreadable(DrumType type);

  static String EffectstypeToHumanReadable(EffectsType type);

  static String CCTypeToHumanReadable(CC_Type type);

  static CC_Type int_to_cc_type(int);

};

#endif
