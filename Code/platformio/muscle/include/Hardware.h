#include <Globals.h>
#include <Effects.h>

class Hardware
{
  public:

  ////////////////////////////////// FOOT SWITCH ////////////////////////
  ///////////////////////////////////////////////////////////////////////

  static const int LOG_BEATS = 0;
  static const int HOLD_CC = 1;
  static const int RESET_TOPO = 2; // resets beat_topography (of all instruments)
  static const int FOOTSWITCH_MODE = 2;
  
  static EffectsType lastEffect[Globals::numInputs];

  static void footswitch_pressed(Instrument *instruments[Globals::numInputs]);

  static void footswitch_released(Instrument *instruments[Globals::numInputs]);

  static void checkFootSwitch(Instrument *instruments[Globals::numInputs]);
  // --------------------------------------------------------------------
};