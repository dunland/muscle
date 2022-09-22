#include <Hardware.h>
#include <settings.h>

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