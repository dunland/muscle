#include <Hardware.h>

void Hardware::footswitch_pressed(Instrument *instruments[Globals::numInputs])
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

  default:
    Serial.println("Footswitch mode not defined!");
    break;
  }
}
// --------------------------------------------------------------------

void Hardware::footswitch_released(Instrument *instruments[Globals::numInputs])
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

void Hardware::checkFootSwitch(Instrument *instruments[Globals::numInputs])
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
      footswitch_pressed(instruments);
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