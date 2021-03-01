#pragma once

#include <Globals.h>
#include <MIDI.h>

#define VIBR 0
#define FOOTSWITCH 2

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
  static const int FOOTSWITCH_MODE = RESET_AND_PROCEED_SCORE;

  static void footswitch_pressed(std::vector<Instrument *> instruments, Score *score);

  static void footswitch_released(std::vector<Instrument *> instruments);

  static void checkFootSwitch(std::vector<Instrument *> instruments, Score* active_score);
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
    for (int i = 0; i<127; i++)
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