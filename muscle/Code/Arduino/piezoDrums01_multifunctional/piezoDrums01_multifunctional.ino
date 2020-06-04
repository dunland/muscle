/*
 * MULTIFUNCTIONAL DRUMS-TO-MIDI-SETUP:
 * ------------------------------------
 * v.1.0
 * ------------------------------------
 * June 2020
 * by David Unland david[at]unland[dot]eu
 * ------------------------------------
 * featuring:
 * global tap tempo by trigger instrument
 * two instruments with note sends (so far: linear from list of phrygian scale notes)
 * one instrument to control notes to be send
 * ------------------------------------
 * INSTRUMENTS:
 * A0 = Tom1      = note trigger 1
 * A1 = Tom2      = note trigger 2
 * A2 = Standtom  = note shifter
 * A3 = cowbell?  = tap tempo trigger
 */

#include <elapsedMillis.h>
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// -------------------------- tap tempo
elapsedMillis firstHitTime;
int bpm = 120;
unsigned long beat_interval = 60000 / 120;
int tapState = 0; // 0 = none; 1 = waiting for first hit; 2 = waiting for second hit

elapsedMillis lastHit_Pin3;
boolean clockSent = false;

// -------------------------- sensitivity

const int noiseFloor = 512; // TODO: create function to assess noiseFloor upon startup
const int max_val = 1000;
const int globalThreshold = 50;

const int PIN_TAP_ACTIVATOR = A3;

static const uint8_t pins[] = {PIN_TAP_ACTIVATOR, A0, A1, A2};
static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN}; // array when using SPRESENSE
static const uint8_t numInputs = 2;
int vals[numInputs];

/* ----------------------------- MUSICAL PARAMETERS ------------------------- */

float rhythm_list[] = {0.03125, 0.04166, 0.0625, 0.0833, 0.0938, 0.125, 0.1667, 0.1875, 0.25, 0.33333, 0.375, 0.5, 0.6667, 0.75, 1}; //{1/32, 1/24, 1/16, 1/12, 3/32, 1/8, 1/6, 3/16, 1/4, 1/3, 3/8, 1/2, 2/3, 3/4, 1/1}
int rhythm_idx[numInputs]; // instrument-specific pointer for rhythms
int notes_list[] = {60, 61, 63, 65, 67, 68, 71}; // phrygian mode: {C, Des, Es, F, G, As, B}
int note_idx[numInputs]; // instrument-specific pointer for notes
int noteState[numInputs]; // 0 = OFF, 1 = ON
boolean noteStateChecked = false;

void setup() {

  Serial.begin(31250); // required clock rate for midi signals
  MIDI.begin(MIDI_CHANNEL_OMNI); // ATTENTION: do these two work together???

  // set all required pins
  for (int i = 0; i < numInputs; i++)
  {
    pinMode(leds[i], OUTPUT);
  }

  // set start notes
  note_idx[1] = 0; // C
  note_idx[2] = 3; // F
}

void loop() {

  //Serial.println(analogRead(A0)); // debugging signal

  /* ------------------------------- TAP TEMPO ------------------------------ */

  // -------------------------------------- tap activator
  if (pinValue(PIN_TAP_ACTIVATOR) > globalThreshold)
  {
    tapState = 1; // start waiting for tap
  }

  // -------------------------------------- tap trigger

  switch (tapState) {
    case 0:
      break;

    case 1:   // waiting for first hit
      for (int i = 0; i < numInputs; i++) // check state of all inputs
      {
        if (pinValue(pins[i]) > globalThreshold)
        {
          firstHitTime = 0; // start recording interval
          tapState = 2; // next: wait for second hit
        }
      }
      break;

    case 2:   // waiting for second hit
      for (int i = 0; i < numInputs; i++) // check state of all inputs
      {
        int val = pinValue(pins[i]);
                  if (val > globalThreshold && firstHitTime > 50)
        {
          if (firstHitTime < 2000) // skip tap evaluation when time was too long
          {
            if (pins[i] == PIN_TAP_ACTIVATOR) // set global tap tempo
            {
              beat_interval = firstHitTime;
              bpm = 60000 / firstHitTime;
              tapState = 0;
            }
            else // set specific fractional beat for that drum
            {
              float spec_drum_rhythm = val / beat_interval;
              int smallest_rhythm_diff = 0;
              for (int r_idx = 0; r_idx < rhythm_list; r_idx++)
              {
                if (abs(1 - (spec_drum_rhythm / rhythm_list[r_idx])) < smallest_rhythm_diff)
                {
                  smallest_rhythm_diff = abs(1 - (spec_drum_rhythm / rhythm_list[r_idx]));
                  rhythm_idx[i] = r_idx;
                }
              }
            }
          }
          else // if time was too long:
          {
            tapState = 0;
          }
        }
      }
      break;
  }

  /* ---------------------------- OTHER TRIGGERS ---------------------------- */
  if (pinValue(pins[3]) > globalThreshold && lastHit_Pin3 > 50)
  {
    lastHit_Pin3 = 0;
    for (int i = 1; i<3; i++) // for instruments 1 and 2
    {
      note_idx[i] = (note_idx[i] + 1) % 7;
    }
  }


  /* -------------------------------- TIME EVENTS --------------------------- */
  // -------------------------------------- send MIDI notes
  for (int i = 0; i < numInputs; i++)
  {
    if (millis() % beat_interval * rhythm_list[rhythm_idx[i]] / 2 == 0) // check noteState twice per beat
    {

      if (!noteStateChecked) // send note only once per cycle
      {
        switch (noteState[i])
        {
          case 0: // turn note on
            MIDI.sendNoteOn(notes_list[note_idx[i]], 127, 2);
            noteState[i] = 1;
            digitalWrite(leds[i], HIGH);
            break;
          case 1: // turn note off
            MIDI.sendNoteOff(notes_list[note_idx[i]], 127, 2);
            noteState[i] = 0;
            digitalWrite(leds[i], LOW);
            break;
        }
        noteStateChecked = true;
      }
    }
    else
    {
      noteStateChecked = false; // reset for next cycle
    }
  }
  // -------------------------------------- blink LED
  // if (millis() % beat_interval < 100) // turns LED on for 100 ms
  // {
  //   digitalWrite(LED_BUILTIN, HIGH);
  // }
  // else
  // {
  //   digitalWrite(LED_BUILTIN, LOW);
  // }

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
