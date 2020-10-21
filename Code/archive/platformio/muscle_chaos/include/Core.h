#ifndef CORE_H
#define CORE_H
#include <Arduino.h>
#include <Globals.h>

class Core
{
public:
  int pinValue(int pinVal_pointer_in, int noiseFloor);

  boolean stroke_detected(int pinDect_pointer_in);
};

#endif