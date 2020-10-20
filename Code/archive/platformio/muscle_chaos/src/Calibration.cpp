#include <Arduino.h>
#include <Globals.h>
#include <Calibration.h>
#include <Debug.h>

void Calibration::calculateNoiseFloor(int numInputs, int pins[])
{
  // calculates the average noise floor out of 400 samples from all inputs

  int led_idx = 0;
  for (int pinNum = 0; pinNum < numInputs; pinNum++)
  {
    Serial.print("calculating noiseFloor for ");
    // Serial.print(DrumtypeToHumanreadable((DrumType)pinNum));

    Serial.print(" ..waiting for stroke");
    if (Debug::use_responsiveCalibration)
    {
      while (analogRead(pins[pinNum]) < 700 + calibration[pinNum][0])
        ; // calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
      Serial.print(" .");
      delay(1000); // should be long enough for drum not to oscillate anymore
    }

    int totalSamples = 0;
    boolean toggleState = false;
    for (int n = 0; n < 400; n++)
    {
      if (n % 100 == 0)
      {
        Serial.print(" . ");
        digitalWrite(Globals::leds[pinNum], toggleState);
        toggleState = !toggleState;
      }
      totalSamples += analogRead(pins[pinNum]);
    }
    noiseFloor[pinNum] = totalSamples / 400;
    digitalWrite(Globals::leds[pinNum], LOW);
    led_idx++;
    Serial.print("noiseFloor = ");
    Serial.println(noiseFloor[pinNum]);
  }

  for (int i = 0; i < Globals::numInputs; i++) // turn Globals::leds off again
  {
    digitalWrite(Globals::leds[i], LOW);
    Debug::output_string[i] = "\t";
  }
}