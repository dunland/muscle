#include <Hardware.h>
#include <Song.h>
#include <Instruments.h>
#include <Calibration.h>
#include <settings.h>
#include <Devtools.h>
#include <settings.h>

//////////////////////// FOOT SWITCH ////////////////////////
/////////////////////////////////////////////////////////////

// std::function<void()> FootSwitch::callbackPressed;
// std::function<void()> FootSwitch::callbackReleased = Song::incrementStep;
FootSwitch* Hardware::footswitch = new FootSwitch(FOOTSWITCH);

// --------------------------------------------------------------
void FootSwitch::poll()
{

  static int switch_state;
  static int last_switch_state = HIGH;
  static unsigned long last_switch_toggle = 1000; // some pre-delay to prevent initial misdetection

  switch_state = digitalRead(FOOTSWITCH);
  if (switch_state != last_switch_state && millis() > last_switch_toggle + 20)
  {
    if (switch_state == LOW)
    {
      Hardware::lcd->setCursor(11, 0);
      Hardware::lcd->print("!");
      Devtools::println_to_console("Footswitch pressed.");

      callbackPressed();
    }
    else
    {
      Hardware::lcd->setCursor(11, 0);
      Hardware::lcd->print(" ");

      Devtools::println_to_console("Footswitch released.");
      callbackReleased();
    }
    last_switch_state = switch_state;
    last_switch_toggle = millis();
  }
}