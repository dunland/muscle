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
  dd200_DelayFeedback = 94,

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
  KP3_Sample_D = 39,

  // whammy CC Channels:
  whammyPedal = 11,
  whammy_CHORDS_2OCT_UP_ON = 44,
  whammy_CHORDS_2OCT_UP_OFF = 65,
  whammy_CHORDS_OCT_UP_ON = 45,
  whammy_CHORDS_OCT_UP_OFF = 66,
  whammy_CHORDS_5TH_UP_ON = 46,
  whammy_CHORDS_5TH_UP_OFF = 67,
  whammy_CHORDS_4TH_UP_ON = 47,
  whammy_CHORDS_4TH_UP_OFF = 68,
  whammy_CHORDS_2ND_UP_ON = 48,
  whammy_CHORDS_2ND_UP_OFF = 69,
  whammy_CHORDS_2ND_DOWN_ON = 49,
  whammy_CHORDS_2ND_DOWN_OFF = 70,
  whammy_CHORDS_4TH_DOWN_ON = 50,
  whammy_CHORDS_4TH_DOWN_OFF = 71,
  whammy_CHORDS_5TH_DOWN_ON = 51,
  whammy_CHORDS_5TH_DOWN_OFF = 72,
  whammy_CHORDS_OCT_DOWN_ON = 52,
  whammy_CHORDS_OCT_DOWN_OFF = 73,
  whammy_CHORDS_DIVEBOMB_ON = 53,
  whammy_CHORDS_DIVEBOMB_OFF = 74,
  whammy_CHORDS_OCT_2OCT_ON = 64,
  whammy_CHORDS_OCT_2OCT_OFF = 85,
  whammy_CHORDS_OCT_10TH_ON = 63,
  whammy_CHORDS_OCT_10TH_OFF = 84,
  whammy_CHORDS_OCT_OCT_ON = 62,
  whammy_CHORDS_OCT_OCT_OFF = 83,
  whammy_CHORDS_OCT_4TH_ON = 61,
  whammy_CHORDS_OCT_4TH_OFF = 82,
  whammy_CHORDS_5TH_OCT_ON = 60,
  whammy_CHORDS_5TH_OCT_OFF = 81,
  whammy_CHORDS_5TH_6TH_ON = 59,
  whammy_CHORDS_5TH_6TH_OFF = 80,
  whammy_CHORDS_5TH_5TH_ON = 58,
  whammy_CHORDS_5TH_5TH_OFF = 79,
  whammy_CHORDS_4TH_5TH_ON = 57,
  whammy_CHORDS_4TH_5TH_OFF = 78,
  whammy_CHORDS_4TH_3RD_ON = 56,
  whammy_CHORDS_4TH_3RD_OFF = 77,

  // KORG Volca Keys Channels:
  volca_Portamento = 5,
  volca_Expression = 11,
  volca_Voice = 40,
  volca_Octave = 41,
  volca_Detune = 42,
  volca_VCO_EG_INT = 43,
  volca_Cutoff = 44,
  volca_VCF_EG_INT = 45,
  volca_LFO_Rate = 46,
  volca_LFO_Pitch_INT = 47,
  volca_LFO_Cutoff_INT = 48,
  volca_EG_Attack = 49,
  volca_EG_DecayRelease = 50,
  volca_EG_Sustain = 51,
  volca_DelayTime = 52,
  volca_DelayFeedback = 53

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
