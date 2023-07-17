#pragma once

#include <Globals.h>
#include <Devtools.h>
#include <MIDI.h>
#include <LiquidCrystal.h>
#include <Encoder.h>

class Song;
class Instrument;

enum FootswitchMode
{
  Log_Beats,
  Hold_CC,
  Reset_Topo, // resets beat_topography (of all instruments)
  Reset_Topo_and_Proceed_Score,
  Experimental,    // hold = mute, release = randomize and increase score step
  Increment_Score, // go to next score step
  No_Footswitch_Mode
};

enum PushbuttonMode
{
  Pb_Edit_Mode,
  Pb_Scroll_Menu
};

struct menu
{
  int number_of_elements; // how many menu elements are there?
  int active_element;     // which menu element is active?
  int pointer = 0;        // pointing at active menu element
  menu *sub_menu;
  menu *parent_menu;
};

class Hardware
{
public:
  ////////////////////////////////// FOOT SWITCH ////////////////////////
  ///////////////////////////////////////////////////////////////////////

  static FootswitchMode footswitch_mode;
  static PushbuttonMode pushbutton_mode;

  static void footswitch_pressed();

  static void footswitch_released();

  static void checkFootSwitch();

  static bool footswitch_is_pressed; // just gives info about state of footswitch
  // --------------------------------------------------------------------

  ////////////////////////////////// LCD ////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  static LiquidCrystal *lcd;
  static void lcd_display();
  static void display_scores();
  static void display_Midi_values(); // display midi values of instruments with FX-Type CC_Change
  static void display_Midi_channels(); // display midi channel of instruments with FX-Type CC_Change
  static menu *lcd_menu;

  // --------------------------------------------------------------------

  ///////////////////////////// ROTARY ENCODER //////////////////////////
  ///////////////////////////////////////////////////////////////////////
  static Encoder *myEnc;
  static int encoder_value;
  static int encoder_count;

  static void checkEncoder();

  /////////////////////////////// PUSHBUTTON ////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  static void checkPushButton();          // checks whether pushbutton is pressed and executes action
  static boolean pushbutton_is_pressed(); // only checks whether pushbutton is pressed

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
      midi_values[i] = 0;
    }
  }

  int midi_channel; // the MIDI Channel to adress this instrument by
  String name;

  boolean notes[127];
  int midi_values[127];

  void sendControlChange(CC_Type cc_type, int val, midi::MidiInterface<HardwareSerial> MIDI); // sets cc_value (used for JSON comm) and sends MIDI-ControlChange

  void sendControlChange(int cc_type, int val, midi::MidiInterface<HardwareSerial> MIDI); // sets cc_value using an integer and sends MIDI-ControlChange // for Random_CC_Effect

  void sendNoteOn(int note, midi::MidiInterface<HardwareSerial> MIDI);

  void sendNoteOff(int note, midi::MidiInterface<HardwareSerial> MIDI);

  void sendProgramChange(int number, midi::MidiInterface<HardwareSerial> MIDI);
};