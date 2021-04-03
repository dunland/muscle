#pragma once

#include <Globals.h>
#include <MIDI.h>
#include <LiquidCrystal.h>
#include <Encoder.h>

#define VIBR 0
#define FOOTSWITCH 2


/* LCD pinout:
1   Vss       GND       
2   Vss       +5V
3   LCD Drive poti power / GND
4   RS        11
5   R/W       GND
6   EN        12
7   D0        --
8   D1        --
9   D2        --
10  D3        --
11  D4        8
12  D5        9
13  D6        4
14  D7        5
15  A         +5V
16  K         GND

*/

// LCD Pins:
#define RS 11
#define EN 12
#define D4 8
#define D5 9
#define D6 4
#define D7 5

// Encoder Pins:
// right - encoder1
// middle - ground
// left - encoder2
#define ENCODER1 3
#define ENCODER2 6
#define PUSHBUTTON 7

class Score;
class Instrument;

class Hardware
{
public:
  ////////////////////////////////// FOOT SWITCH ////////////////////////
  ///////////////////////////////////////////////////////////////////////

  static const int LOG_BEATS = 0;
  static const int HOLD_CC = 1;
  static const int RESET_TOPO = 2; // resets beat_topography (of all instruments)
  static const int RESET_AND_PROCEED_SCORE = 3;
  static const int EXPERIMENTAL = 4; // hold = mute, release = randomize and increase score step
  static const int INCREMENT_SCORE = 5; // hold = mute, release = randomize and increase score step
  static int FOOTSWITCH_MODE;

  static void footswitch_pressed(std::vector<Instrument *> instruments);

  static void footswitch_released(std::vector<Instrument *> instruments);

  static void checkFootSwitch(std::vector<Instrument *> instruments);
  // --------------------------------------------------------------------

  ////////////////////////////////// LCD ////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  static LiquidCrystal *lcd;
  static void display_scores();

  // --------------------------------------------------------------------

  ///////////////////////////// ROTARY ENCODER //////////////////////////
  ///////////////////////////////////////////////////////////////////////
  static Encoder *myEnc;
  static int encoder_value;
  static int encoder_count;

  static void checkEncoder();

  // --------------------------------------------------------------------

  ////////////////////////////// VIBRATION MOTOR ////////////////////////
  ///////////////////////////////////////////////////////////////////////

  static unsigned long motor_vibration_begin;
  static int motor_vibration_duration;

  static void vibrate_motor(unsigned long vibration_duration);

  static void request_motor_deactivation();
};

class Synthesizer
{
public:
  Synthesizer(int midi_channel_)
  {
    midi_channel = midi_channel_;
    Globals::print_to_console("initialized synthesizer with midi channel ");
    Globals::println_to_console(midi_channel);

    // set all notes to false:
    for (int i = 0; i < 127; i++)
    {
      notes[i] = false;
    }
  }

  int midi_channel; // the MIDI Channel to adress this instrument by

  boolean notes[127];

  int osc2_semitone;
  int osc2_tune;
  int mix_level_1;
  int mix_level_2;
  int patch1_depth;
  int patch3_depth;
  int cutoff;
  int resonance;
  int amplevel;
  int attack;
  int sustain;
  int release;
  int delaytime;
  int delaydepth;

  void sendControlChange(CC_Type cc_type, int val, midi::MidiInterface<HardwareSerial> MIDI); // sets cc_value (used for JSON comm) and sends MIDI-ControlChange

  void sendControlChange(int cc_type, int val, midi::MidiInterface<HardwareSerial> MIDI); // sets cc_value using an integer and sends MIDI-ControlChange // for Random_CC_Effect

  void sendNoteOn(int note, midi::MidiInterface<HardwareSerial> MIDI);

  void sendNoteOff(int note, midi::MidiInterface<HardwareSerial> MIDI);
};