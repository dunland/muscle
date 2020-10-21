// ------------------------ Hardware Interfaces -----------------------
#ifndef HARDWARE_H
#define HARDWARE_H

#include <Arduino.h>
#include <Globals.h>

class Hardware
{
public:
  Tsunami tsunami;

  const int LOG_BEATS = 0;
  const int HOLD_CC = 1;
  const int RESET_TOPO = 2; // resets beat_topography (of all instruments)
  const int FOOTSWITCH_MODE = 2;
  boolean footswitch_is_pressed;

  void footswitch_pressed();
  void footswitch_released();
  void checkFootSwitch();
};

#endif