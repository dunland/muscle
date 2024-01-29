#include <Hardware.h>
#include <Song.h>
#include <Instruments.h>
#include <Calibration.h>
#include <settings.h>
#include <Devtools.h>

////////////////////////////////// FOOT SWITCH ////////////////////////
///////////////////////////////////////////////////////////////////////

FootswitchMode Hardware::footswitch_mode = Increment_Score;

// ------------------------------------------------------------------------------
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

    // case (Experimental): // increase step and
    //   Globals::active_song->step++; // go to next score step
    //   Globals::active_song->setup = true;

    //   for (auto &instrument : Drumset::instruments)
    //     instrument->set_effect(Change_CC);
    //   break;

  default:
    Devtools::println_to_console("Footswitch mode not defined!");
    break;
  }
}

// ------------------------------------------------------------------------------
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
        for (auto &midiTarget : instrument->midiTargets)
        {
          instrument->shuffle_cc(midiTarget, false);
          instrument->set_effect(Change_CC);
        }
    }
    break;

  // increment score:
  case (Increment_Score):
    Globals::active_song->increase_step(); // go to next score step
    break;

  default:
    break;
  }
}

// ------------------------------------------------------------------------------
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
    }
    else
    {
      footswitch_released();
      Devtools::println_to_console("Footswitch released.");
    }
    last_switch_state = switch_state;
    last_switch_toggle = millis();
  }
}