#include <elapsedMillis.h>
//#include <MIDI.h>

//MIDI_CREATE_DEFAULT_INSTANCE();

/* TODO:
   TapTempo forgetTime
   TapTempo minTime ?
*/

static const uint8_t numInputs = 4;

boolean firstHit[numInputs] = {false, false, false, false};
elapsedMillis firstHitTime[numInputs];
unsigned long beat_interval[numInputs];
int bpm[numInputs];
boolean clockSent = false;
boolean noteSent[] = {false, false, false, false};

/* ---------------------------------- CUSTOM PARAMETERS --------------------------------- */

int noiseFloor = 512; // typical values: Arduino = 512, SPRESENSE = 260
const int max_val = 1000;
const int globalThreshold = 20; // typical values: Arduino = 50, SPRESENSE = 10-30
static const uint8_t pins[] = {A0, A1, A2, A3};
static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN};
int vals[numInputs];
int note[] = {60, 66, 70, 74};

void setup() {
  Serial2.begin(31250); // required clock rate for midi signals
  while (!Serial2); // wait for Serial port to be ready

  // --------------------------------------- calculate noiseFloor
  //  int pinNum = 0;
  //  int totalSamples = 0;
  //  int led_idx = 0;
  //  for (int n = 0; n < 400; n++)
  //  {
  //    if (n % 100 == 0)
  //    {
  //      digitalWrite(leds[led_idx], HIGH);
  //      led_idx++;
  //    }
  //    pinNum = (pinNum + 1) % numInputs;
  //    totalSamples += analogRead(pins[pinNum]);
  //  }
  //
  //  noiseFloor = totalSamples / 400;
  //printf("noisefloor is at %d", noiseFloor);

  //  MIDI.begin(MIDI_CHANNEL_OMNI); // ATTENTION: does this work together with Serial Comm?

  for (int i = 0; i < 4; i++) // turn LEDs off again
  {
    digitalWrite(leds[i], LOW);
  }


  // --------------------------------------- set pins up
  //Serial.begin(31250); // required clock rate for midi signals

  // set all required pins
  for (int i = 0; i < numInputs; i++)
  {
    pinMode(leds[i], OUTPUT);
    //pinMode(pins[i], INPUT);
  }

}

void loop() {

  //Serial.println(analogRead(A0)); // debugging signal

  for (int i = 0; i < numInputs; i++)
  {
    // -------------------------------------- read button 1
    int val = noiseFloor - analogRead(pins[i]);
    val = abs(val); // positive values only
    val = map(val, 0, max_val, 0, 255); // 8-bit conversion

    // -------------------------------------- tap tempo
    if (val > globalThreshold && !firstHit[i])
    {
      //digitalWrite(LED_BUILTIN, HIGH);
      firstHitTime[i] = 0;
      firstHit[i] = true;
    }

    if (val > globalThreshold && firstHit && firstHitTime[i] > 100) // 100 ms debounce time
    {
      if (firstHitTime[i] < 2000) // skip tap evaluation when time was too long
      {
        beat_interval[i] = firstHitTime[i];
        bpm[i] = 60000 / firstHitTime[i];
      }
      firstHit[i] = false;
    }
  }

  // -------------------------------------- blink Clock LED
  if (millis() % beat_interval[0] < 20) // turns LED on for 100 ms
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }

  // -------------------------------------- MIDI sync
  /* MIDI Clock events are sent at a rate of 24 pulses per quarter note
     one tap beat equals one quarter note
     only one midi clock signal should be send per 24th quarter note
  */
  if (millis() % (beat_interval[0] / 24) == 0)
  {
    if (clockSent)
    {
      clockSent = true;
      Serial2.write(0xF8); // 0xF8 = 11110000 = dataByte for MIDI Timing Clock Realtime Message (https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message)
    }
    //printf(0xF8);
  }
  else
  {
    clockSent = false;
  }

  // send notes
  for (int i = 1; i < numInputs; i++)
  {
    if (millis() % beat_interval[i] == 0 && !noteSent[i])
    {
      noteSent[i] = true;
      //MIDI.sendNoteOn(note[i], 127, 2);
    }
    else
    {
      //MIDI.sendNoteOff(note[i], 127, 2);
      noteSent[i] = false;
    }
  }
}
