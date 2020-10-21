#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h>
#include <Tsunami.h>
#include <MIDI.h>

//MIDI_CREATE_DEFAULT_INSTANCE();
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); // for Serial-specific usage

#define VIBR 0
#define FOOTSWITCH 2

class Globals
{
public:
  static const uint8_t numInputs = 7;
  static uint8_t leds[];
  static int pins[];

  static uint8_t pinAction[];
};

#endif
