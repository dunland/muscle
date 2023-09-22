#pragma once

#include <Globals.h>
#include <MIDI.h>
#include <LiquidCrystal.h>
#include <Encoder.h>
#include <Devtools.h>

class Score;
class Instrument;

enum FootswitchMode
{
  Log_Beats,
  Hold_CC,
  Reset_Topo, // resets beat_topography (of all instruments)
  Reset_Topo_and_Proceed_Score,
  Experimental, // hold = mute, release = randomize and increase score step
  Increment_Score
};

class Knob
{

public:
  Knob(int knob_pin_)
  {
    knob_pin = knob_pin_;
  }

  int knob_pin;

  void poll();

  void pressed();

  void released();
};

class Hardware
{
public:

  static void begin_MIDI();
  static void sendMidiClock();

  ////////////////////////////////// FOOT SWITCH ////////////////////////
  ///////////////////////////////////////////////////////////////////////

  static FootswitchMode footswitch_mode;

  static void footswitch_pressed();

  static void footswitch_released();

  static void checkFootSwitch();
  // --------------------------------------------------------------------

  ////////////////////////////////// LCD ////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  static LiquidCrystal *lcd;
  static void lcd_display();
  static void display_scores();
  static void display_Midi_values(); // display midi values of instruments with FX-Type CC_Change
  static volatile boolean FLAG_CLEAR_LCD; // CAUTION: USED IN INTERRUPTS! DON'T CHANGE WITHOUT STOPPING THEM
  // --------------------------------------------------------------------

  ///////////////////////////// ROTARY ENCODER //////////////////////////
  ///////////////////////////////////////////////////////////////////////
  static Encoder *myEnc;
  static int encoder_value;
  static int encoder_count;
  static int encoder_maxVal;

  static void checkEncoder();

  /////////////////////////////// PUSHBUTTON ////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  static void checkPushButton();          // checks whether pushbutton is pressed and executes action
  static boolean pushbutton_state(); // only checks whether pushbutton is pressed
  static unsigned long last_pushbutton_release;
  // --------------------------------------------------------------------

  ////////////////////////////// VIBRATION MOTOR ////////////////////////
  ///////////////////////////////////////////////////////////////////////

  static unsigned long motor_vibration_begin;
  static int motor_vibration_duration;

  static void vibrate_motor(unsigned long vibration_duration);

  static void request_motor_deactivation();

  //////////////////////////////// DD-200 /////////////////////////////
  /////////////////////////////////////////////////////////////////////

  static int dd_200_midi_interval_map[128];
};

class Synthesizer
{
public:
  Synthesizer(int midi_channel_, String name_="None")
  {
    midi_channel = midi_channel_;
    name = name_;
    Devtools::print_to_console("initialized synthesizer with midi channel ");
    Devtools::println_to_console(midi_channel);

    // set all notes to false:
    for (int i = 0; i < 127; i++)
    {
      notes[i] = false;
    }
  }

  int midi_channel; // the MIDI Channel to adress this instrument by
  String name;

  boolean notes[127];
  int midi_values[127];

  void sendControlChange(CC_Type cc_type, int val); // sets cc_value (used for JSON comm) and sends MIDI-ControlChange

  void sendControlChange(int cc_type, int val); // sets cc_value using an integer and sends MIDI-ControlChange // for Random_CC_Effect

  void sendNoteOn(int note);

  void sendNoteOff(int note);

  void sendProgramChange(int number);
};