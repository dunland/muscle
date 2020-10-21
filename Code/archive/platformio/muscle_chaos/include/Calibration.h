// ------------- sensitivity and instrument calibration ---------------
#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>
#include <Globals.h>

class Calibration
{
public:
  Calibration(int input)
  {
    globalDelayAfterStroke = input;
  }

  int calibration[Globals::numInputs][2]; // [instrument][0:threshold, 1:min_counts_for_signature], will be set in setup()
  int noiseFloor[Globals::numInputs];
  int globalDelayAfterStroke = 10; // 50 ms TODO: assess best timing for each instrument

  // FUNCTIONS:
  void calculateNoiseFloor(int numInputs, int pins[]);
};

#endif