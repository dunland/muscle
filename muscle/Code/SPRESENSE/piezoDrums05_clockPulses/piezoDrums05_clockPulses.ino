/*
   MULTIFUNCTIONAL DRUMS-TO-MIDI-SETUP:
   ------------------------------------
   v.1.0 Setup & Code für Auftritt bei Anachronism in der Schwankhalle am 10.06.2020:
   - etwas wackelige Bühne
   - scheinbar viel Überspielen von Standtom auf Becken
   --> Mögliche Ursachen:
      --> Mapping von Sensor-Read auf 8-bit reduziert Genauigkeit
      --> Metallkästchen stromleitend
   ------------------------------------
   June 2020
   by David Unland david[at]unland[dot]eu
   ------------------------------------
   ------------------------------------
   INSTRUMENTS:
   A0 = Hihat     = tap tempo
   A1 = Crash 1   = change rhythm
   A2 = Crash 2   = change notes
   A3 = Standtom  = play a note

*/

/* -------------------------------------------------------------------------- */
/* --------------------------------- GLOBAL --------------------------------- */
/* -------------------------------------------------------------------------- */
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
const int threshold[] = {30, 170, 170, 60}; // hihat, crash1, ride, Standtom
/*
   instrument, min, max, übersprache tom
   hihat
   crash 1        190   (60)
   ride       60  190   (60)
   tom        60  190
*/

// -------------------------- tap tempo
elapsedMillis timeSinceFirstHit;
int bpm = 0;
int clock_interval = 0; // that is 120 BPM
int tapState = 1; // 0 = none; 1 = waiting for first hit; 2 = waiting for second hit
// int index; // pointer for lists

// elapsedMillis timeSinceLastClockSent;
elapsedMillis timeSinceLastPulse;
// elapsedMillis clockCount;
unsigned long pulseCount = 0;
long clock_sum = 0;
int num_of_taps = 0;

unsigned long lastHit_Pin[numInputs];

/* ----------------------------- MUSICAL PARAMETERS ------------------------- */

// const float rhythm_list[] = {0.03125, 0.04166, 0.0625, 0.0833, 0.0938, 0.125, 0.1667, 0.1875, 0.25, 0.33333, 0.375, 0.5, 0.6667, 0.75, 1, 1.25, 1.3, 1.5, 1.625, 1.6667, 1.75, 1.8125, 1.833, 1.825, 1.9062, 1.9167, 1.9375, 1.95833, 1.96875}; //{1/32, 1/24, 1/16, 1/12, 3/32, 1/8, 1/6, 3/16, 1/4, 1/3, 3/8, 1/2, 2/3, 3/4, 1/1, 5/4, 4/3, 3/2, 9/8, 5/3, 7/4, 19/16, 11/6, 15/8, 49/32, 23/12, 31/16, 47/24, 63/32} ????
// int rhythm_idx[numInputs]; // instrument-specific pointer for rhythms
// int rhythm[numInputs];
int rhythm_frac[numInputs];
//boolean invertRhythmDivision[numInputs];
int notes_list[] = {60, 61, 63, 65, 67, 68, 71}; // phrygian mode: {C, Des, Es, F, G, As, B}
//48, 48, 48, // transition
//53, 54, 56, 58, 60, 61, 64}; // phrygian mode: {F, Fis, Gis, H, C, Des, Es)
int note_idx[numInputs]; // instrument-specific pointer for notes
//int noteState[numInputs]; // 0 = OFF, 1 = ON
// boolean noteStateChecked = false;
uint64_t lastNoteSent[numInputs];
boolean noteSent[numInputs];

int cutoff = 35;
int cutoff_step = 1;

/* -------------------------------------------------------------------------- */
/* ---------------------------------- SETUP --------------------------------- */
/* -------------------------------------------------------------------------- */

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
  int pinNum = 0;
  int totalSamples = 0;
  int led_idx = 0;
  for (int n = 0; n < 400; n++)
  {
    // Serial.print(".");
    if (n % 100 == 0)
    {
      // Serial.println("");
      digitalWrite(leds[led_idx], HIGH);
      led_idx++;
    }
    pinNum = (pinNum + 1) % numInputs;
    totalSamples += analogRead(pins[pinNum]);
  }

  noiseFloor = totalSamples / 400;
  // printf("\nnoiseFloor is at %d\n", noiseFloor);

  for (int i = 0; i < 4; i++) // turn LEDs off again
  {
    digitalWrite(leds[i], LOW);
    //    noteState[i] = 0;
    noteSent[i] = false;
  }

  // --------------------------------------- setup initial values
  // set start notes and rhythms
  note_idx[1] = 0; // C
  note_idx[2] = 3; // F
  note_idx[3] = 0;
  rhythm_frac[1] = 1; // full note
  rhythm_frac[2] = 2; // half note
  lastNoteSent[1] = 0;
  lastNoteSent[2] = 0;

  // clockCount = 0; // let's go!
  timeSinceLastPulse = 0;
}


/* -------------------------------------------------------------------------- */
/* ---------------------------------- LOOP ---------------------------------- */
/* -------------------------------------------------------------------------- */

void loop() {

  //Serial.println(analogRead(A0)); // debugging signal

  /* ------------------------------- TAP TEMPO ------------------------------ */

  // -------------------------------------- tap trigger

  int val = pinValue(pins[0]);
  switch (tapState) {
    //    case 0: // this is for activation of tap tempo listen
    //      // for (int i = 0; i < numInputs; i++)
    //      // {
    //        // printf("\nid= %d\t interval= %d\t bpm= %d", i, clock_interval[i], bpm[i]);
    //      // }
    //      // puts("\n");
    //      tapState = 1;
    //      break;

    case 1:   // waiting for first hit
      if (val > threshold[0]) // Hihat
      {
        if (timeSinceFirstHit > 10000)
        {
          num_of_taps = 0;
          clock_sum = 0;
          // TODO: RHYTHM RESET???????????
          puts("TAP RESET!\n");
        }
        timeSinceFirstHit = 0; // start recording interval
        tapState = 2; // next: wait for second hit
        // index = i; // only look at this pin to be tapped again
        // puts("%d: tap 1 \n");
      }
      break;

    case 2:   // waiting for second hit
      if (val > threshold[0] && timeSinceFirstHit > 300)
      {
        if (timeSinceFirstHit < 2000) // skip tap evaluation when time was too long
        {
          //if (timeSinceFirstHit < 10000) // start new timing if 10 s without tapping
          //{

          // printf("\n%d: tap 2", index);
          // clock_interval = timeSinceFirstHit;
          num_of_taps++;
          clock_sum += timeSinceFirstHit;
          clock_interval = clock_sum / num_of_taps;
          bpm = 60000 / clock_interval;
          tapState = 1;
          // clockCount = 0;
          //}

        }
      }
      if (timeSinceFirstHit > 2000) // forget tap if time was too long
      {
        tapState = 1;
        // Serial.println("too long...");
      }
      // }
      break;
  }

  // printf("bpm: %d\tint: %d\tNotes: %d, %d\tRtm: (%d/%d)\tcut: %d\ttap: %d\n", bpm, clock_interval, notes_list[note_idx[1]], notes_list[note_idx[2]], rhythm_frac[1], rhythm_frac[2], cutoff, tapState);

  /* ---------------------------- OTHER TRIGGERS ---------------------------- */
  // Crash 1: increase rhythm fractions
  if (pinValue(pins[1]) > threshold[1] && millis() >= (lastHit_Pin[1] + 1000))
  {
    lastHit_Pin[1] = millis();
    for (int i = 1; i < 3; i++) // for instruments 1 and 2
    {
      rhythm_frac[i]++;
    }
  }

  // Crash 2: increase note pointer
  if (pinValue(pins[2]) > threshold[2] && millis() >= (lastHit_Pin[2] + 1000))
  {
    lastHit_Pin[2] = millis();
    for (int i = 1; i < 3; i++) // for instruments 1 and 2
    {
      MIDI.sendNoteOff(notes_list[note_idx[i]], 127, 2);
      note_idx[i] = (note_idx[i] + 1) % 7;
    }
    MIDI.sendNoteOff(notes_list[note_idx[3]], 127, 2);
    note_idx[3] = (note_idx[3] + 1) % 3;
  }

  // Standtom: play note
  if (pinValue(pins[3]) > threshold[3])
  {
    lastHit_Pin[3] = millis();
    MIDI.sendNoteOn(notes_list[note_idx[3]] + 12, 127, 2);
  }

  // stop Note from Tom
  if (millis() >= lastHit_Pin[3] + 30) // notes have length of 10 ms
  {
    MIDI.sendNoteOff(notes_list[note_idx[3]] + 12, 127, 2); // sends one octave higher than note[1]
  }

  // Snare: change Cutoff
  //if (pinValue(pins[3]) > threshold[3])
  //{
  // cutoff = cutoff + 2 * cutoff_step;
  // if (cutoff >= 99 || cutoff <= 35) cutoff_step = cutoff_step * -1; // change cutoff step direction
  // MIDI.sendControlChange(74, cutoff, 2);
  // Serial2.write(0xB0);
  // Serial2.write(0x4A); // 74
  // Serial2.write(cutoff, HEX);
  //}


  /* -------------------------------- TIME EVENTS --------------------------- */
  //  // -------------------------------------- blink clock LED
  //
  // if (millis() % clock_interval < 100) // turns LED on for 100 ms
  // {
  //   digitalWrite(leds[0], HIGH);
  // }
  // else
  // {
  //   digitalWrite(leds[0], LOW);
  // }

  // -------------------------------------- MIDI Clock Sync
  /* MIDI Clock events are sent at a rate of 24 pulses per quarter note
     one tap beat equals one quarter note
     only one midi clock signal should be send per 24th quarter note
  */

  if (bpm != 0)
  {

    if (timeSinceLastPulse >= (clock_interval / 32))
    {
      if (pulseCount * 3 % 4 == 0)
      {
        Serial2.write(0xF8);
        //digitalWrite(leds[0], HIGH);
        // timeSinceLastClockSent = 0;
      }

      if (pulseCount % 32 == 0)
      {
        digitalWrite(leds[0], HIGH);
        lastNoteSent[0] = millis();
      }
      if ((pulseCount * rhythm_frac[1]) % 32 == 0)
      {
        MIDI.sendNoteOn(notes_list[note_idx[1]], 127, 2);
        digitalWrite(leds[1], HIGH);
        lastNoteSent[1] = millis();
      }

      if ((pulseCount * rhythm_frac[2]) % 32 == 0)
      {
        MIDI.sendNoteOn(notes_list[note_idx[2]], 127, 2);
        digitalWrite(leds[2], HIGH);
        lastNoteSent[2] = millis();
      }

      timeSinceLastPulse = 0;
      pulseCount++;
    }

    // -------------------------------------- turn notes and LEDs off after 100 ms
    for (int i = 0; i < numInputs; i++)
    {
      if (millis() > lastNoteSent[i] + 100)
      {
        //puts("2\n");
        digitalWrite(leds[i], LOW);
        if (i > 0) MIDI.sendNoteOff(notes_list[note_idx[i]], 127, 2);
      }
    }

  }



  // if (timeSinceLastClockSent >= clock_interval / 24)
  // {
  //   //puts("\ntick!");
  //   timeSinceLastClockSent = 0;
  //   // clockSent = true;
  //   Serial2.write(0xF8); // 0xF8 = 11110000 = dataByte for MIDI Timing Clock Realtime Message (https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message)
  // }

}

int pinValue(int pinNumber_in)
{
  int pinVal_temp = noiseFloor - analogRead(pinNumber_in);
  pinVal_temp = abs(pinVal_temp); // positive values only
  pinVal_temp = map(pinVal_temp, 0, max_val, 0, 255); // 8-bit conversion
  return pinVal_temp;
}

// boolean invertRhythmDivision(int pin_idx_in)
// {
//   boolean temp_invertRhythmDivision;
//   if (clock_interval[pin_idx_in] > clock_interval[0])
//   {
//     rhythm[pin_idx_in] = clock_interval[pin_idx_in] / clock_interval[0];
//     temp_invertRhythmDivision = false;
//   }
//   else
//   {
//     rhythm[pin_idx_in] = clock_interval[0] / clock_interval[pin_idx_in];
//     temp_invertRhythmDivision = true;
//   }
//   return temp_invertRhythmDivision;
// }
