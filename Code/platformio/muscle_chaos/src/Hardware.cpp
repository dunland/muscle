///////////////////////////////// HARDWARE ////////////////////////////
///////////////////////////////////////////////////////////////////////
#include <Globals.h>
#include <Arduino.h>
#include <Hardware.h>


boolean Hardware::footswitch_is_pressed = false;

// -------------------------- FOOTSWITCH PRESSED ----------------------
void Hardware::footswitch_pressed()
{
  switch (FOOTSWITCH_MODE)
  {
  case (LOG_BEATS):
    // set pinMode of all instruments to 3 (record what is being played)
    for (int i = 0; i < Globals::numInputs; i++)
    {
      Swell::lastPinAction[i] = Globals::pinAction[i]; // TODO: lastPinAction seems to be overwritten quickly, so it will not be able to return to its former state. fix this!
      Globals::pinAction[i] = 3;                       // TODO: not for Cowbell?
      for (int j = 0; j < 8; j++)
        Score::set_rhythm_slot[i][j] = false; // reset entire record
    }
    break;

  case (HOLD_CC): // prevents swell_vals to be changed in swell_beat()
    footswitch_is_pressed = true;
    break;

  case (RESET_TOPO): // resets beat_topography (for all instruments)
    for (int i = 0; i < Globals::numInputs; i++)
    {
      // reset 8th-note-topography:
      for (int j = 0; j < 8; j++)
      {
        Score::beat_topography_8.array[i][j] = 0;
      }

      // reset 16th-note-topography:
      for (int j = 0; j < 16; j++)
      {
        Score::beat_topography_16.array[i][j] = 0;
      }
    }

    break;

  default:
    Serial.println("Footswitch mode not defined!");
    break;
  }
}

// -------------------------- FOOTSWITCH RELEASE ----------------------
void Hardware::footswitch_released()
{
  switch (FOOTSWITCH_MODE)
  {
  case (LOG_BEATS):
    for (int i = 0; i < Globals::numInputs; i++)
      Globals::pinAction[i] = Score::initialPinAction[i];
    break;

  case (HOLD_CC):
    footswitch_is_pressed = false;
    break;

  default:
    break;
  }
}

// -------------------------- CHECK FOOTSWITCH ------------------------
void Hardware::checkFootSwitch()
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
      footswitch_pressed();
      Serial.println("Footswitch pressed.");
    }
    else
    {
      footswitch_released();
      Serial.println("Footswitch released.");
    }
    last_switch_state = switch_state;
    last_switch_toggle = millis();
  }
}