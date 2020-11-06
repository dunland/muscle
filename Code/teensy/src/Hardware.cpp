#include <Hardware.h>

////////////////////////////////// FOOT SWITCH ////////////////////////
///////////////////////////////////////////////////////////////////////

void Hardware::footswitch_pressed(std::vector<Instrument*> instruments, Score *score)
{
  switch (FOOTSWITCH_MODE)
  {
  case (LOG_BEATS):
    // set pinMode of all instruments to 3 (record what is being played)
    for (int i = 0; i < Globals::numInputs; i++)
    {
      lastEffect[i] = instruments[i]->effect;
      instruments[i]->effect = FootSwitchLooper; // TODO: not for Cowbell?
      for (int j = 0; j < 8; j++)
        instruments[i]->score.set_rhythm_slot[j] = false; // reset entire record
    }
    break;

  case (HOLD_CC): // prevents swell_vals to be changed in swell_beat()
    Globals::footswitch_is_pressed = true;
    break;

  case (RESET_TOPO): // resets beat_topography (for all instruments)
    for (int i = 0; i < Globals::numInputs; i++)
    {
      // reset 8th-note-topography:
      for (int j = 0; j < 8; j++)
      {
        instruments[i]->topography.a_8[j] = 0;
      }

      // reset 16th-note-topography:
      for (int j = 0; j < 16; j++)
      {
        instruments[i]->topography.a_16[j] = 0;
      }
    }
    break;

  case (RESET_AND_PROCEED_SCORE):
    if (Effect::beat_sum.average_smooth > Effect::beat_sum.activation_thresh)
    {
      Globals::println_to_console("regularity height > 10: reset!");
      Globals::score_state++; // go to next score state
      for (int i = 0; i < Globals::numInputs; i++)
        for (int j = 0; j < 16; j++)
          instruments[i]->topography.a_16[j] = 0;

      Globals::println_to_console("all instrument topographies were reset.");

      for (int j = 0; j < 16; j++)
        Effect::beat_sum.a_16[j] = 0; // reset topography
      Effect::beat_sum.average_smooth = 0;
    }
    
    else
    {
      Globals::print_to_console("regularity too low to proceed.. is at ");
      Globals::println_to_console(Effect::beat_sum.average_smooth);
    }
    break;

  default:
    Serial.println("Footswitch mode not defined!");
    break;
  }
}
// --------------------------------------------------------------------

void Hardware::footswitch_released(std::vector<Instrument*> instruments)
{
  switch (FOOTSWITCH_MODE)
  {
  case (LOG_BEATS):
    for (int i = 0; i < Globals::numInputs; i++)
      instruments[i]->effect = lastEffect[i];
    break;

  case (HOLD_CC):
    Globals::footswitch_is_pressed = false;
    break;

  default:
    break;
  }
}

void Hardware::checkFootSwitch(std::vector<Instrument*> instruments, Score *score)
{

  static int switch_state;
  static int last_switch_state = HIGH;
  static unsigned long last_switch_toggle = 1000; // some pre-delay to prevent initial misdetection
  //static boolean lastPinAction[Globals::numInputs];

  switch_state = digitalRead(FOOTSWITCH);
  if (switch_state != last_switch_state && millis() > last_switch_toggle + 20)
  {
    if (switch_state == LOW)
    {
      footswitch_pressed(instruments, score);
      Serial.println("Footswitch pressed.");
    }
    else
    {
      footswitch_released(instruments);
      Serial.println("Footswitch released.");
    }
    last_switch_state = switch_state;
    last_switch_toggle = millis();
  }
}
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
