#include <Hardware.h>
#include <Score/Score.h>
#include <Instruments.h>
#include <Calibration.h>

////////////////////////////////// FOOT SWITCH ////////////////////////
///////////////////////////////////////////////////////////////////////

FootswitchMode Hardware::footswitch_mode = Increment_Score;
PushbuttonMode Hardware::pushbutton_mode = Pb_Scroll_Menu;

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
    if (Globals::active_score->beat_sum.average_smooth >= Globals::active_score->beat_sum.activation_thresh) // score proceed criterion reached
    {
      Globals::println_to_console("regularity height > 10: reset!");
      Globals::active_score->increase_step(); // go to next score step
      for (auto &instrument : Drumset::instruments)
        for (int j = 0; j < 16; j++)
          instrument->topography.a_16[j] = 0;

      Globals::println_to_console("all instrument topographies were reset.");

      for (int j = 0; j < 16; j++)
        Globals::active_score->beat_sum.a_16[j] = 0; // reset topography
      Globals::active_score->beat_sum.average_smooth = 0;
    }

    else // not enough strokes to proceed yet.
    {
      Globals::print_to_console("regularity too low to proceed.. is at ");
      Globals::println_to_console(Globals::active_score->beat_sum.average_smooth);
    }

    // either way, shuffle instruments with Random_CC_Effect:
    for (auto &instrument : Drumset::instruments)
    {
      if (instrument->effect == Random_CC_Effect)
        instrument->score.ready_to_shuffle = true;
    }

    break;

    // case (Experimental): // increase step and
    //   Globals::active_score->step++; // go to next score step
    //   Globals::active_score->setup = true;

    //   for (auto &instrument : Drumset::instruments)
    //     instrument->set_effect(Change_CC);
    //   break;

  default:
    Globals::println_to_console("Footswitch mode not defined!");
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
      Globals::active_score->increase_step(); // go to next score step
    }
    break;

  // increment score:
  case (Increment_Score):
    Globals::active_score->increase_step(); // go to next score step
    break;

  default:
    break;
  }
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
      Globals::println_to_console("Footswitch pressed.");
    }
    else
    {
      footswitch_released();
      Globals::println_to_console("Footswitch released.");
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

  // Machine-State-dependent display:
  switch (Globals::machine_state)
  {
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
      static boolean running_mode = true;

      if (millis() > (last_running_state_switch + 3000))
      {
        running_mode = !running_mode;
        Hardware::lcd->clear();
        last_running_state_switch = millis();
      }

      if (running_mode == true) // mode A: display scores
        Hardware::display_scores();
      else // mode B: display Midi Values
        Hardware::display_Midi_values();
    }
    else
    {
      // display both score and midi vals:
      Hardware::display_scores();
      Hardware::display_Midi_values();
    }

    break;
  }
  case Calibration:
  {
    //level 1: display all instruments
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
    break;
  }
  }
}

void Hardware::display_scores()
{
  // active score display:
  lcd->setCursor(0, 1);
  lcd->print(Globals::score_list[Globals::active_score_pointer]->name);

  lcd->setCursor(14, 1);
  lcd->print(Globals::active_score->step);
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
      Hardware::lcd->print(int(Drumset::instruments[i]->midi_settings.cc_val));

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
      Globals::active_score->proceed_to_next_score();
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

//////////////////////// SYNTHESIZER CLASS ////////////////////////////
///////////////////////////////////////////////////////////////////////

// sets cc_value (for JSON communication) and sends MIDI-ControlChange:
void Synthesizer::sendControlChange(CC_Type cc_type, int val, midi::MidiInterface<HardwareSerial> MIDI)
{
  switch (cc_type)
  {
  case Osc2_semitone:
    osc2_semitone = val;
    break;

  case Osc2_tune:
    osc2_tune = val;
    break;

  case Mix_Level_1:
    mix_level_1 = val;
    break;

  case Mix_Level_2:
    mix_level_2 = val;
    break;

  case Patch_1_Depth:
    patch1_depth = val;
    break;

  case Patch_3_Depth:
    patch3_depth = val;
    break;

  case Cutoff:
    cutoff = val;
    break;

  case Resonance:
    resonance = val;
    break;

  case Amplevel:
    amplevel = val;
    break;

  case Attack:
    attack = val;
    break;

  case Sustain:
    sustain = val;
    break;

  case Release:
    release = val;
    break;

  case DelayTime:
    delaytime = val;
    break;

  case DelayDepth:
    delaydepth = val;
    break;

  default:
    break;
  }

  if (cc_type < 0)
  {
    Globals::println_to_console("could not send MIDI CC Command: CC_Type not defined.");
  }
  else
  {
    MIDI.sendControlChange(int(cc_type), val, midi_channel);
  }
}

void Synthesizer::sendControlChange(int cc_type, int val, midi::MidiInterface<HardwareSerial> MIDI)
{

  // TODO: complete this list. May lead to incomplete monitoring otherwise..
  switch (cc_type)
  {
  case Osc2_semitone:
    osc2_semitone = val;
    break;

  case Osc2_tune:
    osc2_tune = val;
    break;

  case Mix_Level_1:
    mix_level_1 = val;
    break;

  case Mix_Level_2:
    mix_level_2 = val;
    break;

  case Patch_1_Depth:
    patch1_depth = val;
    break;

  case Patch_3_Depth:
    patch3_depth = val;
    break;

  case Cutoff:
    cutoff = val;
    break;

  case Resonance:
    resonance = val;
    break;

  case Amplevel:
    amplevel = val;
    break;

  case Attack:
    attack = val;
    break;

  case Sustain:
    sustain = val;
    break;

  case Release:
    release = val;
    break;

  case DelayTime:
    delaytime = val;
    break;

  case DelayDepth:
    delaydepth = val;
    break;

  default:
    break;
  }

  if (cc_type < 0)
  {
    Globals::println_to_console("could not send MIDI CC Command: CC_Type not defined.");
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