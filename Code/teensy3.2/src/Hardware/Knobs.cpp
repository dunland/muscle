#include <Hardware.h>
#include <Devtools.h>

// ------------------------------------------------------------------------------
void Knob::poll()
{

  static int switch_state;
  static int last_switch_state = HIGH;
  static unsigned long last_switch_toggle = 1000; // some pre-delay to prevent initial misdetection

  switch_state = digitalRead(knob_pin);
  if (switch_state != last_switch_state && millis() > last_switch_toggle + 20)
  {
    if (switch_state == LOW)
    {
      Devtools::println_to_console("Knob pressed.");
    }
    else
    {
      Devtools::println_to_console("Knob released.");
    }
    last_switch_state = switch_state;
    last_switch_toggle = millis();
  }
}