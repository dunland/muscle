#include <Hardware.h>
#include <Song.h>
#include <Instruments.h>
#include <Calibration.h>
#include <settings.h>

////////////////////////////////// FOOT SWITCH ////////////////////////
///////////////////////////////////////////////////////////////////////

FootswitchMode Hardware::footswitch_mode = Increment_Score;
PushbuttonMode Hardware::pushbutton_mode = Pb_Scroll_Menu;
bool Hardware::footswitch_is_pressed = false;

void Hardware::footswitch_pressed()
{
  lcd->setCursor(11, 0);
  lcd->print("!");

  switch (footswitch_mode)
  {
  case (Log_Beats):
    // set pinMode of all instruments to 3 (record what is being played)
    for (auto &instrument : Drumset::instruments)
    {
      instrument->lastEffect = instrument->effect;
      instrument->effect = FootSwitchLooper; // TODO: not for Cowbell?
      for (int j = 0; j < 8; j++)
        instrument->score.set_rhythm_slot[j] = false; // reset entire record
    }
    break;

  case (Hold_CC): // prevents swell_vals to be changed in swell_beat()
    Globals::footswitch_is_pressed = true;
    break;

  case (Reset_Topo): // resets beat_topography (for all instruments)
    for (auto &instrument : Drumset::instruments)
    {
      // reset 8th-note-topography:
      for (int j = 0; j < 8; j++)
      {
        instrument->topography.a_8[j] = 0;
      }

      // reset 16th-note-topography:
      for (int j = 0; j < 16; j++)
      {
        instrument->topography.a_16[j] = 0;
      }
    }
    break;

  case (Reset_Topo_and_Proceed_Score):
    if (Globals::active_song->beat_sum.average_smooth >= Globals::active_song->beat_sum.activation_thresh) // score proceed criterion reached
    {
      Devtools::println_to_console("regularity height > 10: reset!");
      Globals::active_song->increase_step(); // go to next score step
      for (auto &instrument : Drumset::instruments)
        for (int j = 0; j < 16; j++)
          instrument->topography.a_16[j] = 0;

      Devtools::println_to_console("all instrument topographies were reset.");

      for (int j = 0; j < 16; j++)
        Globals::active_song->beat_sum.a_16[j] = 0; // reset topography
      Globals::active_song->beat_sum.average_smooth = 0;
    }

    else // not enough strokes to proceed yet.
    {
      Devtools::print_to_console("regularity too low to proceed.. is at ");
      Devtools::println_to_console(Globals::active_song->beat_sum.average_smooth);
    }

    // either way, shuffle instruments with Random_CC_Effect:
    for (auto &instrument : Drumset::instruments)
    {
      if (instrument->effect == Random_CC_Effect)
        instrument->score.ready_to_shuffle = true;
    }

    break;

  case (No_Footswitch_Mode):
    break;
    // case (Experimental): // increase step and
    //   Globals::active_score->step++; // go to next score step
    //   Globals::active_score->setup = true;

    //   for (auto &instrument : Drumset::instruments)
    //     instrument->set_effect(Change_CC);
    //   break;

  default:
    Devtools::println_to_console("Footswitch mode not defined!");
    break;
  }
}

void Hardware::footswitch_released()
{

  lcd->setCursor(11, 0);
  lcd->print(" ");

  switch (footswitch_mode)
  {
  case (Log_Beats):
    for (auto &instrument : Drumset::instruments)
      instrument->effect = instrument->lastEffect;
    break;

  case (Hold_CC):
    Globals::footswitch_is_pressed = false;
    break;

  case (Experimental):
    for (auto &instrument : Drumset::instruments)
    {
      instrument->shuffle_cc(false);
      instrument->set_effect(Change_CC);
      Globals::active_song->increase_step(); // go to next score step
    }
    break;

  // increment score:
  case (Increment_Score):
    Globals::active_song->increase_step(); // go to next score step
    break;

  default:
    break;
  }

  Serial.println("footswitch released");
}

void Hardware::checkFootSwitch()
{

  static int switch_state;
  static int last_switch_state = HIGH;
  static unsigned long last_switch_toggle = 1000; // some pre-delay to prevent initial misdetection

  switch_state = digitalRead(FOOTSWITCH);
  if (switch_state != last_switch_state && millis() > last_switch_toggle + 20)
  {
    if (switch_state == LOW)
    {
      footswitch_pressed();
      Devtools::println_to_console("Footswitch pressed.");
      footswitch_is_pressed = true;
    }
    else
    {
      footswitch_released();
      Devtools::println_to_console("Footswitch released.");
      footswitch_is_pressed = false;
    }
    last_switch_state = switch_state;
    last_switch_toggle = millis();
  }
}

////////////////////////////////// LCD ////////////////////////////////
///////////////////////////////////////////////////////////////////////
LiquidCrystal *Hardware::lcd = new LiquidCrystal(RS, EN, D4, D5, D6, D7);
menu *Hardware::lcd_menu = new menu;

void Hardware::lcd_display()
{
  // clear LCD display twice per second:
  static unsigned long last_clear = 0;
  if (millis() > last_clear + 500)
  {
    lcd->clear();
    last_clear = millis();
  }

  // ------------- Machine-State-dependent display: -------------------
  switch (Globals::machine_state)
  {
  // -------------------------- RUNNING -------------------------------
  case Running:
  {
    // switch if any instrument has CC mode:
    int instruments_with_CC_mode = 0;
    for (uint8_t i = 0; i < Drumset::instruments.size(); i++)
    {
      if (Drumset::instruments[i]->effect == Change_CC)
      {
        instruments_with_CC_mode++;
      }
    }

    // start to toggle when CC-occupied display area gets too large
    if (instruments_with_CC_mode > 3)
    {
      static unsigned long last_running_state_switch = 0;
      static int running_mode = 0;

      if (millis() > (last_running_state_switch + 3000))
      {
        running_mode = (running_mode + 1) % 3;
        Hardware::lcd->clear();
        last_running_state_switch = millis();
      }

      switch (running_mode)
      {
      case 0: // mode A: display scores
        Hardware::display_scores();
        break;
      case 1: // mode B: display Midi Values
        Hardware::display_Midi_values();
        break;
      case 2: // mode C: show CC-channel
        Hardware::display_Midi_channels();
        break;
      default:
        running_mode = 0;
        break;
      }
    }
    else
    {
      // display both score and midi vals:
      Hardware::display_scores();
      Hardware::display_Midi_values();
    }

    break;
  }
  // -------------------------- CALIBRATION ---------------------------
  case Calibration:
  {
    // level 1: display all instruments
    for (uint8_t i = 0; i < Drumset::instruments.size(); i++)
    {
      if (Drumset::instruments[i]->effect == Change_CC)
      {
        Hardware::lcd->setCursor(((i % 4) * 4), int(i >= 4));
        Hardware::lcd->print(Globals::DrumtypeToHumanreadable(Drumset::instruments[i]->drumtype));
        Hardware::lcd->setCursor(((Hardware::lcd_menu->pointer % 4) * 4), int(Hardware::lcd_menu->pointer >= 4));
        Hardware::lcd->print("→");
      }
    }
    break;

  default:
    /* define what to display elsewhere... */
    break;
  }
  }
}

// ------------------------- print stuff ------------------------------
void Hardware::display_scores()
{
  // score name
  lcd->setCursor(0, 1);
  lcd->print(Globals::active_song->name);

  // step
  lcd->setCursor(14, 1);
  lcd->print(Globals::active_song->step);
}

// display midi values of instruments with FX-Type CC_Change
void Hardware::display_Midi_values()
{
  int pos = 0;
  for (uint8_t i = 0; i < Drumset::instruments.size(); i++)
  {
    if (Drumset::instruments[i]->effect == Change_CC)
    {
      Hardware::lcd->setCursor(((pos % 4) * 4), int(pos >= 4));
      Hardware::lcd->print(Globals::DrumtypeToHumanreadable(Drumset::instruments[i]->drumtype)[0]);
      Hardware::lcd->setCursor(((pos % 4) * 4) + 1, int(pos >= 4));
      Hardware::lcd->print(int(Drumset::instruments[i]->midi.cc_val));

      pos++;
    }
  }
}

// display midi CHANNEL of instruments with FX-Type CC_Change
void Hardware::display_Midi_channels(){
int pos = 0;

  for (uint8_t i = 0; i < Drumset::instruments.size(); i++)
  {
    if (Drumset::instruments[i]->effect == Change_CC)
    {
      Hardware::lcd->setCursor(((pos % 4) * 4), int(pos >= 4));
      Hardware::lcd->print("|");
      Hardware::lcd->setCursor(((pos % 4) * 4) + 1, int(pos >= 4));
      Hardware::lcd->print(int(Drumset::instruments[i]->midi.cc_chan));
      pos++;
    }
  }
}

// --------------------------------------------------------------------

///////////////////////////// ROTARY ENCODER //////////////////////////
///////////////////////////////////////////////////////////////////////
Encoder *Hardware::myEnc = new Encoder(ENCODER1, ENCODER2);
int Hardware::encoder_value;
int Hardware::encoder_count = 0;

void Hardware::checkEncoder()
{
  static long encoder_oldPosition = -999;
  static int encoder_previous_count = 0;
  long encoder_newPosition = myEnc->read();

  // ROTARY ENCODER:
  if (encoder_oldPosition != encoder_newPosition)
  {

    // rising value:
    if (encoder_oldPosition > encoder_newPosition)
    {
      if (encoder_count > 0)
        encoder_count--;
      encoder_oldPosition = encoder_newPosition;
      if (encoder_count < 100 && encoder_previous_count >= 100)
        lcd->clear();
      encoder_previous_count = encoder_count;
    }

    // sinking value:
    else
    {
      if (encoder_count < 127)
        encoder_count++;
      encoder_oldPosition = encoder_newPosition;
    }
  }
}

///////////////////////////// PUSH BUTTON ///////////////////////////
void Hardware::checkPushButton()
{
  if (pushbutton_is_pressed())
  {
    switch (Globals::machine_state)
    {
    case Running:
      Globals::active_song->proceed_to_next_score();
      break;

    case Calibration:

      switch (pushbutton_mode)
      {
      case Pb_Edit_Mode: // change value and leave edit mode
        encoder_value = encoder_count;
        // Calibration::set_value(encoder_value);
        pushbutton_mode = Pb_Scroll_Menu;
        break;
      case Pb_Scroll_Menu: // select menu and go to edit mode
        Hardware::lcd_menu->pointer = (Hardware::lcd_menu->pointer + 1) % Hardware::lcd_menu->number_of_elements;
        pushbutton_mode = Pb_Edit_Mode;
        break;
      }
      break;

    default:
      break;
    }
  }
}

boolean Hardware::pushbutton_is_pressed()
{
  static unsigned long lastPush = 0;
  if (digitalRead(PUSHBUTTON) == LOW && millis() > lastPush + 200)
  {
    lastPush = millis();
    lcd->setCursor(11, 0);
    lcd->print("!");
    return true;
  }
  else
  {
    // lcd->setCursor(11, 0);
    // lcd->print(" ");
    return false;
  }
}

// --------------------------------------------------------------------

////////////////////////////// VIBRATION MOTOR ////////////////////////
///////////////////////////////////////////////////////////////////////
unsigned long Hardware::motor_vibration_begin = 0;
int Hardware::motor_vibration_duration = 0;

void Hardware::vibrate_motor(unsigned long vibration_duration_input)
{
  motor_vibration_begin = millis();
  motor_vibration_duration = vibration_duration_input;
  digitalWrite(VIBR, HIGH);
}

void Hardware::request_motor_deactivation() // turn off vibration and MIDI notes
{
  if (millis() > motor_vibration_begin + motor_vibration_duration)
    digitalWrite(VIBR, LOW);
}

///////////////////////// DD-200 MIDI-to-BPM map //////////////////////

int Hardware::dd_200_midi_interval_map[128] =
    // ATTENTION: not so sure which of these tables is BPM and which is interval, but like this it seems to be working nicely:
    // {
    //     60000, 30000, 1000, 741, 496, 373, 300, 250,
    //     214, 188, 167, 150, 137, 126, 116, 108,
    //     101, 94, 89, 84, 79, 75, 72, 69,
    //     66, 63, 60, 58, 56, 54, 52, 50,
    //     49, 47, 46, 44, 43, 42, 41, 40,
    //     39, 38, 37, 36, 35, 34, 34, 33,
    //     32, 31, 31, 30, 30, 29, 29, 28,
    //     27, 27, 27, 26, 26, 25, 25, 24,
    //     24, 24, 23, 23, 23, 22, 22, 22,
    //     21, 21, 21, 20, 20, 20, 20, 19,
    //     19, 19, 19, 18, 18, 18, 18, 18,
    //     17, 17, 17, 17, 17, 16, 16, 16,
    //     16, 16, 16, 15, 15, 15, 15, 15,
    //     15, 15, 14, 14, 14, 14, 14, 14,
    //     14, 14, 13, 13, 13, 13, 13, 13,
    //     13, 13, 12, 12, 12, 12, 12, 12};

    {1, 2, 42, 81, 121, 161, 200, 240,
     280, 319, 359, 399, 438, 478, 518, 557,
     597, 637, 676, 716, 756, 795, 835, 875,
     914, 954, 994, 1030, 1070, 1110, 1150, 1190, 1230,
     1270, 1310, 1350, 1390, 1430, 1460, 1500, 1540,
     1580, 1620, 1660, 1700, 1740, 1780, 1820, 1860,
     1900, 1940, 1980, 2020, 2060, 2100, 2140, 2180,
     2220, 2260, 2300, 2340, 2380, 2420, 2460, 2500,
     2540, 2580, 2620, 2650, 2690, 2730, 2770, 2810,
     2850, 2890, 2930, 2970, 3010, 3050, 3090, 3130,
     3170, 3210, 3250, 3290, 3330, 3370, 3410, 3450,
     3490, 3530, 3570, 3610, 3650, 3690, 3730, 3770,
     3800, 3840, 3880, 3920, 3960, 4000, 4040, 4080,
     4120, 4160, 4200, 4240, 4280, 4320, 4360, 4400,
     4440, 4480, 4520, 4560, 4600, 4640, 4680, 4720,
     4760, 4800, 4840, 4880, 4920, 4960, 5000};

//////////////////////// SYNTHESIZER CLASS ////////////////////////////
///////////////////////////////////////////////////////////////////////

// sets cc_value (for JSON communication) and sends MIDI-ControlChange:
void Synthesizer::sendControlChange(CC_Type cc_type, int val, midi::MidiInterface<HardwareSerial> MIDI)
{
  static int previous_val = -1;
  midi_values[cc_type] = val; // store value

  if (cc_type < 0)
  {
    Devtools::println_to_console("could not send MIDI CC Command: CC_Type not defined.");
  }
  else if (val != previous_val)
  {
    MIDI.sendControlChange(int(cc_type), val, midi_channel);
    previous_val = val;
  }
}

void Synthesizer::sendControlChange(int cc_type, int val, midi::MidiInterface<HardwareSerial> MIDI)
{
  midi_values[cc_type] = val;

  if (cc_type < 0)
  {
    Devtools::println_to_console("could not send MIDI CC Command: CC_Type not defined.");
  }
  else
  {
    MIDI.sendControlChange(int(cc_type), val, midi_channel);
  }
}

void Synthesizer::sendNoteOn(int note, midi::MidiInterface<HardwareSerial> MIDI)
{
  notes[note] = true; // remember that note is turned on
  MIDI.sendNoteOn(note, 127, midi_channel);
}

void Synthesizer::sendNoteOff(int note, midi::MidiInterface<HardwareSerial> MIDI)
{
  notes[note] = false; // remember that note is turned off
  MIDI.sendNoteOff(note, 127, midi_channel);
}

void Synthesizer::sendProgramChange(int number, midi::MidiInterface<HardwareSerial> MIDI)
{
  MIDI.sendProgramChange(number, midi_channel);
}