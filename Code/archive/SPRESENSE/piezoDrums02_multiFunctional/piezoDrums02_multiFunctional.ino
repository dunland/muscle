/*
   MULTIFUNCTIONAL DRUMS-TO-MIDI-SETUP:
   ------------------------------------
   v.1.0
   ------------------------------------
   June 2020
   by David Unland david[at]unland[dot]eu
   ------------------------------------
   featuring:
   global tap tempo by trigger instrument
   two instruments with note sends (so far: linear from list of phrygian scale notes)
   one instrument to control notes to be send
   ------------------------------------
   INSTRUMENTS:
   A0 = cowbell?  = tap tempo trigger
   A1 = Tom1      = note trigger 1
   A2 = Tom2      = note trigger 2
   A3 = Standtom  = note shifter

*/

#include <elapsedMillis.h>
#include <MIDI.h>

// MIDI_CREATE_DEFAULT_INSTANCE();
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

// -------------------------- pins
static const uint8_t numInputs = 4;

const int PIN_TAP_ACTIVATOR = A0;

static const uint8_t pins[] = {PIN_TAP_ACTIVATOR, A1, A2, A3};
static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN1, LED_BUILTIN2, LED_BUILTIN3}; // array when using SPRESENSE
int vals[numInputs];

// -------------------------- sensitivity
int noiseFloor; // typical values: Arduino = 512, SPRESENSE = 260
const int max_val = 1023;
//const int globalThreshold = 30; // typical values: 60-190 for toms (SPRESENSE, 3.3V)
const int threshold[] = {50, 100, 100, 100}; // 60-190 for toms

// -------------------------- tap tempo
elapsedMillis firstHitTime;
int bpm[] = {120, 120, 120, 120};
unsigned long beat_interval[] = {500, 500, 500, 500}; // that is 120 BPM
int tapState = 0; // 0 = none; 1 = waiting for first hit; 2 = waiting for second hit
int index; // pointer for lists

elapsedMillis lastHit_Pin3; // pin3 has an independent function (note shift)
elapsedMillis lastClockSent = false;

/* ----------------------------- MUSICAL PARAMETERS ------------------------- */

// float rhythm_list[] = {0.04166, 0.0625, 0.0833, 0.0938, 0.125, 0.1667, 0.1875, 0.25, 0.33333, 0.375, 0.5, 0.6667, 0.75, 1}; //{1/24, 1/16, 1/12, 3/32, 1/8, 1/6, 3/16, 1/4, 1/3, 3/8, 1/2, 2/3, 3/4, 1/1}
// int rhythm_idx[numInputs]; // instrument-specific pointer for rhythms
int notes_list[] = {60, 61, 63, 65, 67, 68, 71}; // phrygian mode: {C, Des, Es, F, G, As, B}
int note_idx[numInputs]; // instrument-specific pointer for notes
int noteState[numInputs]; // 0 = OFF, 1 = ON
// boolean noteStateChecked = false;

elapsedMillis lastNoteSent[numInputs];
boolean noteSent[numInputs];

void setup() {

  // Serial2 for UART2 pin on SPRESENSE --> "Serial" can be used for USB communication!
  // Serial2.begin(31250); // required clock rate for midi signals
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(115200);

  //--------------------------------------- setup all required pins
  for (int i = 0; i < numInputs; i++)
  {
    pinMode(leds[i], OUTPUT);
  }
  // --------------------------------------- calculate noiseFloor
  // --------------------------------------- calculate noiseFloor
  int pinNum = 0;
  int totalSamples = 0;
  int led_idx = 0;
  for (int n = 0; n < 400; n++)
  {
    Serial.print(".");
    if (n % 100 == 0)
    {
      Serial.print("|");
      digitalWrite(leds[led_idx], HIGH);
      led_idx++;
    }
    pinNum = (pinNum + 1) % numInputs;
    totalSamples += analogRead(pins[pinNum]);
  }

  noiseFloor = totalSamples / 400;
  printf("\nnoiseFloor is at %d", noiseFloor);

  for (int i = 0; i < 4; i++) // turn LEDs off again
  {
    digitalWrite(leds[i], LOW);
    noteState[i] = 0;
    lastNoteSent[i] = 0;
    noteSent[i] = false;
  }

  // set start notes
  note_idx[1] = 0; // C
  note_idx[2] = 3; // F
}

void loop() {

  //Serial.println(analogRead(A0)); // debugging signal

  /* ------------------------------- TAP TEMPO ------------------------------ */

  // -------------------------------------- tap activator
  //if (pinValue(PIN_TAP_ACTIVATOR) > threshold[i])
  //{
  //tapState = 1; // start waiting for tap
  //}

  // -------------------------------------- tap trigger

  switch (tapState) {
    case 0: // this is for activation of tap tempo listen
      for (int i = 0; i < numInputs; i++)
      {
        printf("\nid= %d\t interval= %d\t bpm= %d", i, beat_interval[i], bpm[i]);
      }
      puts("\n");
      tapState = 1;
      break;

    case 1:   // waiting for first hit
      for (int i = 0; i < numInputs; i++) // check state of all inputs
      {
        if (pinValue(pins[i]) > threshold[i])
        {
          firstHitTime = 0; // start recording interval
          tapState = 2; // next: wait for second hit
          index = i; // only look at this pin to be tapped again
          printf("%d: tap 1", i);
        }
      }
      break;

    case 2:   // waiting for second hit
      // for (int i = 0; i < numInputs; i++) // check state of all inputs
      // {
      int val = pinValue(pins[index]); // read only pin
      if (val > threshold[index] && firstHitTime > 150)
      {
        if (firstHitTime < 2000) // skip tap evaluation when time was too long
        {
          // if (tappedPin == PIN_TAP_ACTIVATOR) // set global tap tempo
          // {
          printf("\n%d: tap 2", index);
          beat_interval[index] = firstHitTime;
          bpm[index] = 60000 / firstHitTime;
          tapState = 0;
          // }
          // else // set specific fractional beat for that drum
          // {
          // float spec_drum_rhythm = val / beat_interval;
          // int smallest_rhythm_diff = 0;
          // for (int r_idx = 0; r_idx < rhythm_list; r_idx++)
          // {
          //   if (abs(1 - (spec_drum_rhythm / rhythm_list[r_idx])) < smallest_rhythm_diff)
          //   {
          //     smallest_rhythm_diff = abs(1 - (spec_drum_rhythm / rhythm_list[r_idx]));
          //     rhythm_idx[i] = r_idx;
          //   }
          // }
          // }
        }
      }
      if (firstHitTime > 2000) // forget tap if time was too long
      {
        tapState = 0;
        Serial.println("too long...");
      }
      // }
      break;
  }

  /* ---------------------------- OTHER TRIGGERS ---------------------------- */
  if (pinValue(pins[3]) > threshold[3] && lastHit_Pin3 > 150)
  {
    lastHit_Pin3 = 0;
    for (int i = 1; i < 3; i++) // for instruments 1 and 2
    {
      note_idx[i] = (note_idx[i] + 1) % 7;
    }
  }

  /* -------------------------------- TIME EVENTS --------------------------- */
  // -------------------------------------- send MIDI notes and blink LED
  for (int i = 0; i < numInputs; i++)
  {

    if (lastNoteSent[i] > beat_interval[i])
    {
      lastNoteSent[i] = 0;
      if (!noteSent[i]) // send only once per cycle
      {
        digitalWrite(leds[i], HIGH);
        if (i > 0) MIDI.sendNoteOn(notes_list[note_idx[i]], 127, 2);
        lastNoteSent[i] = 0;
        noteSent[i] = true;
      }
    }

    if (lastNoteSent[i] > 100)
    {
      digitalWrite(leds[i], LOW);
      noteSent[i] = false;
      if (i > 0) MIDI.sendNoteOff(notes_list[note_idx[i]], 127, 2);
    }

  }
  // for (int i = 0; i < numInputs; i++)
  // {
  //   //    //if (millis() % beat_interval * rhythm_list[rhythm_idx[i]] / 2 == 0) // check noteState twice per beat
  //   if (millis() % beat_interval[i] == 0) //TODO: stattdessen noteInterval[i] % beat_interval --> (elapsedMillis)noteInterval bei jedem Schlag auf 0 zurücksetzen
  //   {
  //     if (!noteStateChecked) // send note only once per cycle
  //     {
  //       switch (noteState[i])
  //       {
  //         case 0: // turn note on
  //           MIDI.sendNoteOn(notes_list[note_idx[i]], 127, 2);
  //           noteState[i] = 1;
  //           digitalWrite(leds[i], HIGH);
  //           break;
  //         case 1: // turn note off
  //           MIDI.sendNoteOff(notes_list[note_idx[i]], 127, 2);
  //           noteState[i] = 0;
  //           digitalWrite(leds[i], LOW);
  //           break;
  //       }
  //       noteStateChecked = true;
  //     }
  //   }
  //   else
  //   {
  //     noteStateChecked = false; // reset for next cycle
  //   }
  // }
//  // -------------------------------------- blink clock LED
//
//  if (millis() % beat_interval[0] < 100) // turns LED on for 100 ms
//  {
//    digitalWrite(leds[0], HIGH);
//  }
//  else
//  {
//    digitalWrite(leds[0], LOW);
//  }


  // -------------------------------------- MIDI Clock Sync
  /* MIDI Clock events are sent at a rate of 24 pulses per quarter note
     one tap beat equals one quarter note
     only one midi clock signal should be send per 24th quarter note
  */
  //  Serial.print(millis());
  //  Serial.print(" interval:");
  //  Serial.print(beat_interval[0]);
  //  Serial.print(" bpm: ");
  //  Serial.print(bpm[0]);
  //  Serial.print(" interval/(6*32): ");
  //  Serial.print(beat_interval[0] / (6 * 32));
  //  Serial.print(" ms_modulo: ");
  //  Serial.println(millis() % (beat_interval[0] / (6 * 32)));

  // if (millis() % (beat_interval / 24) == 0 && !clockSent)
  //if (millis() % (beat_interval[0] / (6 * 24) == 0 && !clockSent))
  if (lastClockSent > beat_interval[0] / 24)
  {
    //puts("\ntick!");
    lastClockSent = 0;
    // clockSent = true;
    Serial2.write(0xF8); // 0xF8 = 11110000 = dataByte for MIDI Timing Clock Realtime Message (https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message)
  }

}

int pinValue(int pinNumber_in)
{
  int pinVal_temp = noiseFloor - analogRead(pinNumber_in);
  pinVal_temp = abs(pinVal_temp); // positive values only
  pinVal_temp = map(pinVal_temp, 0, max_val, 0, 255); // 8-bit conversion
  return pinVal_temp;
}
