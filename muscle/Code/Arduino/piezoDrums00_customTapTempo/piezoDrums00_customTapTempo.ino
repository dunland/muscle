#include <elapsedMillis.h>
#inlcude <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

/* TODO:
   TapTempo forgetTime
   TapTempo minTime ?
*/

boolean firstHit = false;
elapsedMillis firstHitTime;
unsigned long beat_interval;
int bpm;
boolean clockSent = false;

/* ---------------------------------- CUSTOM PARAMETERS --------------------------------- */

const int noiseFloor = 512; // TODO: create function to assess noiseFloor upon startup
const int max_val = 1000;
const int globalThreshold = 50;

const int PIN_TAP_ACTIVATOR = A3;
const int PIN_TAP_TRIGGER = A0;
static const uint8_t pins[] = {A0, A1, A2};
static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN}; // array when using SPRESENSE
static const uint8_t numInputs = 1;
int vals[numInputs];

void setup() {

  Serial.begin(31250); // required clock rate for midi signals
  MIDI.begin(MIDI_CHANNEL_OMNI); // ATTENTION: do these two work together???

  // set all required pins
  for (int i = 0; i < numInputs; i++)
  {
    pinMode(leds[i], OUTPUT);
  }

}

void loop() {

  //Serial.println(analogRead(A0)); // debugging signal

  /* --------------------------------- TAP TEMPO --------------------------------- */

  // -------------------------------------- tap activator
  int val = pinValue(PIN_TAP_ACTIVATOR);
  boolean waitingForTap = false;
  
  if (val > globalThreshold)
  {
    waitingForTap = true;
  }

  // -------------------------------------- tap trigger

  val = pinValue(PIN_TAP_TRIGGER);
  // hit:
  if (val > globalThreshold && !firstHit && waitingForTap)
  {
    //digitalWrite(LED_BUILTIN, HIGH);
    firstHitTime = 0;
    firstHit = true;
  }

  // hit:
  if (val > globalThreshold && firstHit && firstHitTime > 50) // 50 ms debounce time
  {
    if (firstHitTime < 2000) // skip tap evaluation when time was too long
    {
      beat_interval = firstHitTime;
      bpm = 60000 / firstHitTime;
    }
    firstHit = false;
    waitingforTap = false; // prevent multiple tap detections
  }

  // -------------------------------------- blink LED
  if (millis() % beat_interval < 100) // turns LED on for 100 ms
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }

  // -------------------------------------- MIDI Clock Sync
  /* MIDI Clock events are sent at a rate of 24 pulses per quarter note
     one tap beat equals one quarter note
     only one midi clock signal should be send per 24th quarter note
  */
  if (millis() % (beat_interval / 24) == 0 && !clockSent)
  {
    clockSent = true;
    Serial.write(0xF8); // 0xF8 = 11110000 = dataByte for MIDI Timing Clock Realtime Message (https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message)
  }
  else
  {
    clockSent = false;
  }

}

int pinValue(int pinNumber_in)
{
  int pinVal_temp = noiseFloor - analogRead(pinNumber_in);
  pinVal_temp = abs(pinVal_temp); // positive values only
  pinVal_temp = map(pinVal_temp, 0, max_val, 0, 255); // 8-bit conversion
  return val;
}
