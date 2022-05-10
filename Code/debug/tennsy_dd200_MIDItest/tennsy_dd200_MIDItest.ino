// TX2 = pin8

#include <MIDI.h>
midi::MidiInterface<HardwareSerial> MIDI((HardwareSerial &)Serial2); // same as MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

void setup() {
  while (!Serial);
  Serial.begin(9600);

}

void loop() {
  static int val = 0;
  val = (val + 1) % 127;
  MIDI.sendControlChange(94, val, 3);

  Serial.println(val);
  Serial.println(millis());

  delay(10);
  

}
