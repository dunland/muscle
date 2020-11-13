#pragma once

#include <Globals.h>

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

  static void footswitch_pressed(std::vector<Instrument*> instruments);

  static void footswitch_released(std::vector<Instrument*> instruments);

  static void checkFootSwitch(std::vector<Instrument*> instruments);
  // --------------------------------------------------------------------

  ////////////////////////////// VIBRATION MOTOR ////////////////////////
  ///////////////////////////////////////////////////////////////////////

  static unsigned long motor_vibration_begin;
  static int motor_vibration_duration;
  
  static void vibrate_motor(unsigned long vibration_duration);

  static void request_motor_deactivation();

};