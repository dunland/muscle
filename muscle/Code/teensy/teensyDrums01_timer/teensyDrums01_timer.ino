/*
   MULTIFUNCTIONAL DRUMS-TO-MIDI-SETUP:
   ------------------------------------
   14.07.2020
   by David Unland david[at]unland[dot]eu
   ------------------------------------
   ------------------------------------
   INSTRUMENTS:
   A0 =   = tap tempo
   A1 =   = change rhythm
   A2 =   = change notes
   A3 =   = play a note

*/

/* --------------------------------------------------------------------- */
/* ------------------------------- GLOBAL ------------------------------ */
/* --------------------------------------------------------------------- */
// #include <MIDI.h>

// MIDI_CREATE_DEFAULT_INSTANCE();
// MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

// -------------------------- pins
static const uint8_t numInputs = 4;
static const uint8_t pins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
// static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN1, LED_BUILTIN2, LED_BUILTIN3}; // array when using SPRESENSE
static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN};
int min_crossings_for_signature[] = {1, 1, 1, 1, 1, 1, 1, 1}; // TODO: find characteristic signatures and insert here
int vals[numInputs];

// ------------------ variables for interrupt timers ------------------
IntervalTimer pinMonitor; // reads pins every 1 ms
IntervalTimer masterClock; // 1 bar
volatile int crossings[numInputs];
volatile unsigned long lastPinActiveTime[numInputs];
volatile unsigned long firstPinActiveTime[numInputs];

// ------------------ timing and rhythm tracking ------------------
int current_beat_pos; // always stores the current position in the beat

// int bpm = 0;
// int tapInterval = 500; // 0.5 s per beat for 120 BPM
// int tapState = 1; // 0 = none; 1 = waiting for first hit; 2 = waiting for second hit


// ------------- sensitivity and instrument calibration -----------------
int noiseFloor; // typical values: Arduino (5V) = 512, SPRESENSE (3.3V) = 260
const int max_val = 1023;
//const int globalThreshold = 30; // typical values: 60-190 for toms (SPRESENSE, 3.3V)
// const int threshold[] = {30, 170, 170, 60}; // hihat, crash1, ride, Standtom
const int threshold[] = {60, 60, 60, 60, 60, 60, 60, 60};
int globalDelayAfterStroke = 50; // 50 ms TODO: assess best timing

/* --------------------------- MUSICAL PARAMETERS ---------------------- */

// const float rhythm_list[] = {0.03125, 0.04166, 0.0625, 0.0833, 0.0938, 0.125, 0.1667, 0.1875, 0.25, 0.33333, 0.375, 0.5, 0.6667, 0.75, 1, 1.25, 1.3, 1.5, 1.625, 1.6667, 1.75, 1.8125, 1.833, 1.825, 1.9062, 1.9167, 1.9375, 1.95833, 1.96875}; //{1/32, 1/24, 1/16, 1/12, 3/32, 1/8, 1/6, 3/16, 1/4, 1/3, 3/8, 1/2, 2/3, 3/4, 1/1, 5/4, 4/3, 3/2, 9/8, 5/3, 7/4, 19/16, 11/6, 15/8, 49/32, 23/12, 31/16, 47/24, 63/32} ????
// int rhythm_frac[numInputs];
boolean rhythm_slot[numInputs][32];
int notes_list[] = {60, 61, 63, 65, 67, 68, 71}; // phrygian mode: {C, Des, Es, F, G, As, B}
int note_idx[numInputs]; // instrument-specific pointer for notes
// uint64_t lastNoteSent[numInputs];
// boolean noteSent[numInputs];

/* --------------------------------------------------------------------- */
/* ---------------------------------- SETUP ---------------------------- */
/* --------------------------------------------------------------------- */

void setup() {

  Serial.begin(9600);
  while(!Serial);
  // MIDI.begin(MIDI_CHANNEL_OMNI);

  //--------------------------------------- initialize pins and arrays
  for (int i = 0; i < numInputs; i++)
  {
    pinMode(leds[i], OUTPUT);
    crossings[i] = 0;
  }
 // ----------------------------- calculate noiseFloor ------------------------
  // calculates the average noise floor out of 400 samples from all inputs
  int pinNum = 0;
  int totalSamples = 0;
  int led_idx = 0;
  boolean toggleState = false;

  for (int n = 0; n < 400; n++)
  {
    // Serial.print(".");
    if (n % 100 == 0)
    {
      // Serial.println("");
      digitalWrite(LED_BUILTIN, toggleState);
      led_idx++;
      toggleState = !toggleState;
    }
    pinNum = (pinNum + 1) % numInputs;
    totalSamples += analogRead(pins[pinNum]);
  }
  noiseFloor = totalSamples / 400;
  digitalWrite(LED_BUILTIN, LOW);

  for (int i = 0; i < 4; i++) // turn LEDs off again
  {
    digitalWrite(leds[i], LOW);
    noteSent[i] = false;
  }

  // --------------------------------------- setup initial values
  // set start notes and rhythms
  note_idx[1] = 0; // C
  note_idx[2] = 3; // F
  note_idx[3] = 0;
  rhythm_frac[1] = 1; // full note
  rhythm_frac[2] = 2; // half note

  // ------------------------------ begin timer --------------------------------
  pinMonitor.begin(samplePins, 1000);  // sample pin every 1 millisecond
  masterClock.begin(masterClockTimer, tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds
}

/* --------------------------------------------------------------------- */
/* -------------------------- TIMED INTERRUPTS ------------------------- */
/* --------------------------------------------------------------------- */

void samplePins()
{
  for (int pinNum = 0; pinNum < numInputs; pinNum++)
  {
    if (pinValue(pinNum) > threshold[pinNum])
    {
      if (crossings[pinNum] < 1) firstPinActiveTime[pinNum] = millis();
      lastPinActiveTime[pinNum] = millis();
      crossings[pinNum]++;
    }
  } 
}

volatile unsigned long masterClockCount = 0; // 4*32 = 128 masterClockCount per cycle
volatile unsigned long beatCount = 0;
// volatile int bar_step; // 0-32
volatile int currentStep; // 0-32
int nextStep = 0;  // will be reset when timer restarts

void masterClockTimer()
{
  /*
  * timing              fraction          bar @ 120 BPM    bar @ 180 BPM
  * 1 bar             = 1               = 2 s       = 1.3 s
  * 1 beatCount       = 1/32 bar        = 62.5 ms   = 41.7 ms 
  * stroke precision  = 1/4 clockPulse  = 15.625 ms = 10.4166 ms
  */

 masterClockCount++;

 if (masterClockCount % (32*4) == 0)
 {
   beatCount++;
 }

  // evaluate current position of beat in bar for stroke precision
  if ((masterClockCount % 128) >= nextStep - 2)
  {
    currentStep = nextStep;
    nextStep += 4;
  }

}

/* --------------------------------------------------------------------- */
/* ------------------------------- LOOP -------------------------------- */
/* --------------------------------------------------------------------- */

void loop() 
{
  for (int i = 0; i < numInputs; i++)
  {
    if (stroke_detected(pins[i])) // evaluates pins for activity repeatedly
    {
      considerPinAction(pins[i]); // what to do? tapTempo? first oder second hit? rhythmical coded triggers?
    }

    executePinAction(pins[i], pinAction); // actually perform the action...
  }

  // timed action:
  static int last_beat_pos = 0;

  noInterrupts();
  current_beat_pos = beatCount % 32;
  interrupts();

  if (current_beat_pos != last_beat_pos)
  {
    for (int i = 0; i < numInputs; i++)
    {
      if (rhythm_slot[pins[i]][current_beat_pos] == true)
      {
        /* perform action, e.g. send MIDI note */
      }
    }
  }
  current_beat_pos = last_beat_pos;

}

/* --------------------------------------------------------------------- */
/* ----------------------------- FUNCTIONS ----------------------------- */
/* --------------------------------------------------------------------- */

int pinValue(int pinNumber_in)
{
  int pinVal_temp = noiseFloor - analogRead(pinNumber_in);
  pinVal_temp = abs(pinVal_temp); // positive values only
  pinVal_temp = map(pinVal_temp, 0, max_val, 0, 255); // 8-bit conversion
  return pinVal_temp;
}

boolean stroke_detected(int assess_pin)
{
  static int crossingsCopy[numInputs];
  static unsigned long lastPinActiveTimeCopy[numInputs];
  static unsigned long firstPinActiveTimeCopy[numInputs];
  static int lastValue[numInputs]; // for LED toggle
  static boolean toggleState = false; // for LED toggle

  // for (int i = 0; i < numInputs; i++)
  // {
    noInterrupts();
    lastPinActiveTimeCopy[assess_pin] = lastPinActiveTime[assess_pin];
    firstPinActiveTimeCopy[assess_pin] = firstPinActiveTime[assess_pin];
    interrupts();

    // if (millis() > lastPinActiveTimeCopy[assess_pin] + globalDelayAfterStroke) // get crossings only X ms after last hit
    if (millis() > firstPinActiveTimeCopy[assess_pin] + globalDelayAfterStroke) // get crossings only X ms after first hit ??
    {
      noInterrupts();
      crossingsCopy[assess_pin] = crossings[assess_pin];
      crossings[assess_pin] = 0;
      interrupts();

      if (crossingsCopy[assess_pin] >= min_crossings_for_signature[assess_pin])
      {

        /* STROKE DETECTED. EXECUTE EITHER FIRST OR SECOND HIT ACTION 
        * by setting a flag for executePinActions() later 
        */

        // Serial output and LED blink:
        if (crossingsCopy[assess_pin] != lastValue[assess_pin]) toggleState = !toggleState;
        digitalWrite(LED_BUILTIN, toggleState);
        Serial.print(millis());
        Serial.print("\t");
        Serial.print(pins[assess_pin]);
        Serial.print("\t");
        Serial.print(crossingsCopy[assess_pin]);
        Serial.print("\t");
        Serial.println(threshold[assess_pin]);
      }
      lastValue[assess_pin] = crossingsCopy[assess_pin];
      return true;
    }
    else
    {
      {
        return false;
      }
    }
    
  // }
}

void considerPinAction(int considered_pin)
{
  static int pinTapAmount[numInputs];
  static int firstHitTime[numInputs];
  static int secondHitTime[numInputs];
  /* e.g. firstHit --> store current clockPulse
   * secondHit --> store current clockPulse, get time difference
   */

  // relative clock tapping:
  if (pinTapAmount[considered_pin] == 1)
  {
    noInterrupts();
    firstHitTime[considered_pin] = currentStep;
    interrupts();
    // TODO: fill rhythm_slot[action_pin][current_beat_pos] ? 

  }
  else if (pinTapAmount[considered_pin] == 2)
  {
    noInterrupts();
    secondHitTime[considered_pin] = currentStep;
    interrupts();
    // TODO: fill rhythm_slot[action_pin][current_beat_pos] ? 
    
    // TODO: diff = secondHitTime[considered_pin] - firstHitTime[considered_pin];
  }

  // poly-rhythmic figures here?
}

void executePinAction(int action_pin, int action)
{

  switch (action)
  {
  case 0:
    /* code
     * fill rhythm_slot[action_pin][current_beat_pos] ?
     */
    break;

  case 1:
    /* code
     * fill rhythm_slot[action_pin][current_beat_pos] ?
     */
    break;
  
  default:
    break;
  }
}