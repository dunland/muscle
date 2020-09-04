/* --------------------------------------------------------------------- */
/* ----------------------------- FUNCTIONS ----------------------------- */
/* --------------------------------------------------------------------- */

////////////////////////////// CALCULATE NOISEFLOOR ///////////////////
///////////////////////////////////////////////////////////////////////
void calculateNoiseFloor()
{
  // calculates the average noise floor out of 400 samples from all inputs

  int led_idx = 0;
  for (int pinNum = 0; pinNum < numInputs; pinNum++)
  {
    Serial.print("calculating noiseFloor for ");
    Serial.print(names[pinNum]);
    Serial.print(" ..waiting for stroke");
    if (responsiveCalibration)
    {
      while (analogRead(pins[pinNum]) < 700 + calibration[pinNum][0])
        ; // calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
      Serial.print(" .");
      delay(1000); // should be long enough for drum not to oscillate anymore
    }

    int totalSamples = 0;
    boolean toggleState = false;
    for (int n = 0; n < 400; n++)
    {
      //Serial.print(".");
      if (n % 100 == 0)
      {
        Serial.print(" . ");
        digitalWrite(leds[pinNum], toggleState);
        toggleState = !toggleState;
      }
      totalSamples += analogRead(pins[pinNum]);
    }
    noiseFloor[pinNum] = totalSamples / 400;
    digitalWrite(leds[pinNum], LOW);
    led_idx++;
    Serial.print("noiseFloor = ");
    Serial.println(noiseFloor[pinNum]);
  }

  for (int i = 0; i < numInputs; i++) // turn LEDs off again
  {
    digitalWrite(leds[i], LOW);
    output_string[i] = "\t";
  }
}
// --------------------------------------------------------------------

////////////////////////////////// FOOT SWITCH ////////////////////////
///////////////////////////////////////////////////////////////////////
void checkFootSwitch()
{

  static int switch_state;
  static int last_switch_state = HIGH;
  static unsigned long last_switch_toggle = 1000; // some pre-delay to prevent initial misdetection
  static boolean lastPinAction[numInputs];

  switch_state = digitalRead(FOOTSWITCH);
  if (switch_state != last_switch_state && millis() > last_switch_toggle + 20)
  {
    if (switch_state == LOW)
    {
      Serial.println("Footswitch pressed.");
      // set pinMode of all instruments to 3 (record what is being played)
      for (int i = 0; i < numInputs; i++)
      {
        lastPinAction[i] = pinAction[i]; // TODO: lastPinAction seems to be overwritten quickly, so it will not be able to return to its former state. fix this!
        pinAction[i] = 3;                // TODO: not for Cowbell?
        for (int j = 0; j < 8; j++)
          set_rhythm_slot[i][j] = false; // reset entire record
      }
    }
    else
    {
      for (int i = 0; i < numInputs; i++)
        pinAction[i] = initialPinAction[i];

      Serial.println("Footswitch released.");
    }
    last_switch_state = switch_state;
    last_switch_toggle = millis();
  }
}
// --------------------------------------------------------------------

/////////////////////////// general pin reading ///////////////////////
///////////////////////////////////////////////////////////////////////

int pinValue(int pinVal_pointer_in)
{
  return abs(noiseFloor[pinVal_pointer_in] - analogRead(pins[pinVal_pointer_in]));
}
// --------------------------------------------------------------------

// ///////////////////////// PRINT NORMALIZED VALUES //////////////////
///////////////////////////////////////////////////////////////////////
void printNormalizedValues(boolean printNorm_criterion)
{
  // useful debugger for column-wise output of raw/normalised values:

  if (printNorm_criterion == true)
  {
    static unsigned long lastMillis;
    if (millis() != lastMillis)
    {
      for (int i = 0; i < numInputs; i++)
      {
        static int countsCopy[numInputs];
        //noInterrupts();
        //countsCopy[i] = counts[i];
        //interrupts();
        Serial.print(pins[i]);
        Serial.print("\t");
        Serial.print(pinValue(i));
        Serial.print("\t");
        //Serial.print(", ");
        //Serial.print(countsCopy[i]);
      }
      Serial.println("");
    }
    lastMillis = millis();
  }
}
// --------------------------------------------------------------------

///////////////////// SET STRING FOR PLAY LOGGING /////////////////////
///////////////////////////////////////////////////////////////////////

// int velocity[numInputs]; // this should be done in the swell section, but is needed in print section already... :/

void setInstrumentPrintString(int incoming_i, int incoming_pinAction)
{
  switch (incoming_pinAction)
  {

  case 1: // monitor: just print what is being played
    if (incoming_i == KICK)
      output_string[incoming_i] = "■\t"; // Kickdrum
    else if (incoming_i == COWBELL)
      output_string[incoming_i] = "▲\t"; // Crash
    else if (incoming_i == STANDTOM1)
      output_string[incoming_i] = "□\t"; // Standtom
    else if (incoming_i == STANDTOM2)
      output_string[incoming_i] = "O\t"; // Standtom
    else if (incoming_i == HIHAT)
      output_string[incoming_i] = "x\t"; // Hi-Hat
    else if (incoming_i == TOM1)
      output_string[incoming_i] = "°\t"; // Tom 1
    else if (incoming_i == SNARE)
      output_string[incoming_i] = "※\t"; // Snaredrum
    else if (incoming_i == TOM2)
      output_string[incoming_i] = "o\t"; // Tom 2
    else if (incoming_i == RIDE)
      output_string[incoming_i] = "xx\t"; // Ride
    else if (incoming_i == CRASH1)
      output_string[incoming_i] = "-X-\t"; // Crash
    else if (incoming_i == CRASH2)
      output_string[incoming_i] = "-XX-\t"; // Crash
    break;

  case 2: // toggle beat slot
    if (incoming_i == KICK)
      output_string[incoming_i] = "■\t"; // Kickdrum
    else if (incoming_i == COWBELL)
      output_string[incoming_i] = "▲\t"; // Crash
    else if (incoming_i == STANDTOM1)
      output_string[incoming_i] = "□\t"; // Standtom
    else if (incoming_i == STANDTOM2)
      output_string[incoming_i] = "O\t"; // Standtom
    else if (incoming_i == HIHAT)
      output_string[incoming_i] = "x\t"; // Hi-Hat
    else if (incoming_i == TOM1)
      output_string[incoming_i] = "°\t"; // Tom 1
    else if (incoming_i == SNARE)
      output_string[incoming_i] = "※\t"; // Snaredrum
    else if (incoming_i == TOM2)
      output_string[incoming_i] = "o\t"; // Tom 2
    else if (incoming_i == RIDE)
      output_string[incoming_i] = "xx\t"; // Ride
    else if (incoming_i == CRASH1)
      output_string[incoming_i] = "-X-\t"; // Crash
    else if (incoming_i == CRASH2)
      output_string[incoming_i] = "-XX-\t"; // Crash
    break;

  case 3: // add an ! if pinAction == 3 (replay logged rhythm)
    if (incoming_i == KICK)
      output_string[incoming_i] = "!■\t"; // Kickdrum
    else if (incoming_i == COWBELL)
      output_string[incoming_i] = "!▲\t"; // Crash
    else if (incoming_i == STANDTOM1)
      output_string[incoming_i] = "!□\t"; // Standtom
    else if (incoming_i == STANDTOM2)
      output_string[incoming_i] = "!O\t"; // Standtom
    else if (incoming_i == HIHAT)
      output_string[incoming_i] = "!x\t"; // Hi-Hat
    else if (incoming_i == TOM1)
      output_string[incoming_i] = "!°\t"; // Tom 1
    else if (incoming_i == SNARE)
      output_string[incoming_i] = "!※\t"; // Snaredrum
    else if (incoming_i == TOM2)
      output_string[incoming_i] = "!o\t"; // Tom 2
    else if (incoming_i == RIDE)
      output_string[incoming_i] = "!xx\t"; // Ride
    else if (incoming_i == CRASH1)
      output_string[incoming_i] = "!-X-\t"; // Crash
    else if (incoming_i == CRASH2)
      output_string[incoming_i] = "!-XX-\t"; // Crash
    break;

    // case 5: // print velocity for repeated MIDI notes in "swell" mode
    //   output_string[incoming_i] = velocity[incoming_i];
    //   output_string[incoming_i] += "\t";
    //   break;
  }
}

// --------------------------------------------------------------------

// ///////////////////////// STROKE DETECTION /////////////////////////
// ////////////////////////////////////////////////////////////////////
boolean stroke_detected(int pinDect_pointer_in)
{
  static unsigned long lastPinActiveTimeCopy[numInputs];
  static unsigned long firstPinActiveTimeCopy[numInputs];
  static int lastValue[numInputs];    // for LED toggle
  static boolean toggleState = false; // for LED toggle

  noInterrupts();
  lastPinActiveTimeCopy[pinDect_pointer_in] = lastPinActiveTime[pinDect_pointer_in];
  firstPinActiveTimeCopy[pinDect_pointer_in] = firstPinActiveTime[pinDect_pointer_in];
  interrupts();

  if (millis() > lastPinActiveTimeCopy[pinDect_pointer_in] + globalDelayAfterStroke) // get counts only X ms after LAST hit
  //if (millis() > firstPinActiveTimeCopy[pinDect_pointer_in] + globalDelayAfterStroke)
  //get counts only X ms after FIRST hit ??
  {
    noInterrupts();
    countsCopy[pinDect_pointer_in] = counts[pinDect_pointer_in];
    counts[pinDect_pointer_in] = 0;
    interrupts();

    // ---------------------------- found significant count!
    if (countsCopy[pinDect_pointer_in] >= calibration[pinDect_pointer_in][1])
    {
      // LED blink:
      //if (countsCopy[pinDect_pointer_in] != lastValue[pinDect_pointer_in]) toggleState = !toggleState;
      //digitalWrite(LED_BUILTIN, toggleState);
      lastValue[pinDect_pointer_in] = countsCopy[pinDect_pointer_in];

      // countsCopy[pinDect_pointer_in] = 0;

      return true;
    }
    else // timing ok but no significant counts
    {
      return false;
    }
  }
  else // TODO: timing not ok (obsolete: will always be ok!)
  {
    {
      return false;
    }
  }
}
// --------------------------------------------------------------------

// //////////////////// SOUND SWELL: INITIALIZATION ///////////////////
// ////////////////////////////////////////////////////////////////////

/* SOUND SWELL ALGORITHM: ---------------------------------------------

RECORD:

-------X-------X-------X-------X------- hits
-------|-------|-------|-------|------- 
-------1-------2-------3-------4------- num_of_swell_taps
-------|-------|-------|-------|-------
------502-----510-----518-----526------ beatCount (beatPos)
-------|-------|-------|-------|-------
-------|-------|------prev----curr-----
-------|-------|-------|-------|-------
-------|-------8-------16------32------ swell_beatPos_sum = sum + (curr - prev)
-------|---8---|---8---|---8---|------- swell_stroke_interval = sum/num
------v++-----v++-----v++-----v++------ increase velocity


PLAY:

-------|-------|-------|-------|-------
-------0-------1-------2-------3------- beatStep
-------|-------|-------|-------|-------
-------v-----(v↓)----(v↓)----(v↓)------ decrease velocity
-------🎵-------🎵-------🎵------🎵------- play MIDI note

---------------------------------------------------------------------*/

int num_of_swell_taps[numInputs];     // will be used in both swell_rec() and swell_beat(). serves as velocity for MIDI notes.
int swell_stroke_interval[numInputs]; // will be needed for timed replay
int swell_state[numInputs];
int velocity[numInputs];
unsigned long swell_beatPos_sum[numInputs];

void swell_init() // initialize arrays for swell functions (run in setup())
{
  for (int i = 0; i < numInputs; i++)
  {
    swell_state[i] = 1;
    num_of_swell_taps[i] = 0;
    velocity[i] = 0;
    swell_beatPos_sum[i] = 0;
  }
}
// --------------------------------------------------------------------

// -------------------- SOUND SWELL: RECORD STROKES -------------------
// --------------------------------------------------------------------

void swell_rec(int instr) // remembers beat stroke position
{
  /* works pretty much just like the tapTempo, but repeats the triggered drums on external MIDI instrument (-> swell_beat() in TIMED INTERVALS) */
  static unsigned long previous_swell_beatPos[numInputs];

  if (swell_state[instr] == 1) // first hit
  {
    // grab the current beat counter
    noInterrupts();
    previous_swell_beatPos[instr] = beatCount;
    interrupts();
    swell_state[instr] = 2;
  }
  else if (swell_state[instr] == 2) // second hit
  {
    num_of_swell_taps[instr]++; // will define the velocity for replayed MIDI notes
    velocity[instr] += 2;
    unsigned long current_swell_beatPos;

    noInterrupts();
    current_swell_beatPos = beatCount;
    interrupts();

    // get diff to first (how many beats were in between?):
    swell_beatPos_sum[instr] += current_swell_beatPos - previous_swell_beatPos[instr];
    // average all hits and repeat at that rate:
    swell_stroke_interval[instr] = swell_beatPos_sum[instr] / num_of_swell_taps[instr];
    previous_swell_beatPos[instr] = current_swell_beatPos;
  }
}
// --------------------------------------------------------------------

// -------------------- SOUND SWELL: REPLAY STROKES -------------------
// --------------------------------------------------------------------

void swell_beat(int instr) // updates once a 32nd-beat-step
{
  static int swell_beatStep[numInputs]; // increases with beatCount and initiates action.

  // remember update moment in beat:
  noInterrupts();
  unsigned long updateMoment = beatCount;
  interrupts();

  // ready to play MIDI again?
  swell_beatStep[instr] = (swell_beatStep[instr] + 1) % swell_stroke_interval[instr];
  //Serial.print("swStp");
  //Serial.print(swell_beatStep[instr]);
  //Serial.print("\t");

  //velocity[instr] = (velocity[instr] > 127) ? 127 : velocity[instr]; // max velocity = 127
  if (swell_beatStep[instr] == 0) // on swell beat
  {
    output_string[instr] = String(velocity[instr]);
    MIDI.sendControlChange(07, velocity[instr], 2); // TODO: set volume/cutoff/.. instead of velocity
    MIDI.sendNoteOn(notes_list[instr], velocity[instr], 2);

    if (velocity[instr] > 0 && swell_state[instr] == 2)
    {
      velocity[instr]--; // decrease velocity
    }
    else // reset swell if velocity == 0:
    {
      velocity[instr] = 0;
      num_of_swell_taps[instr] = 0;
      swell_beatPos_sum[instr] = 0;      
      swell_state[instr] = 1;
    }
    // velocity[instr] = (velocity[instr] > 0) * (velocity[instr] - 1); // decrease velocity only if > 0
  }
  else // stop MIDI
  {
    MIDI.sendNoteOff(notes_list[instr], velocity[instr], 2);
  }
}
// --------------------------------------------------------------------

////////////////////////////// TAP TEMPO //////////////////////////////
///////////////////////////////////////////////////////////////////////

void getTapTempo()
{
  static unsigned long timeSinceFirstTap = 0;
  static int tapState = 1;
  static int num_of_taps = 0;
  static int clock_sum = 0;

  switch (tapState)
  {
    //    case 0: // this is for activation of tap tempo listen
    //      // for (int i = 0; i < numInputs; i++)
    //      // {
    //      // }
    //      // Serial.print("\n");
    //      tapState = 1;
    //      break;

  case 1:                                     // first hit
    if (millis() > timeSinceFirstTap + 10000) // reinitiate tap if not used for ten seconds
    {
      num_of_taps = 0;
      clock_sum = 0;
      Serial.println("-----------TAP RESET!-----------\n");
    }
    timeSinceFirstTap = millis(); // record time of first hit
    tapState = 2;                 // next: wait for second hit

    break;

  case 2: // second hit

    if (millis() < timeSinceFirstTap + 2000) // only record tap if interval was not too long
    {
      num_of_taps++;
      clock_sum += millis() - timeSinceFirstTap;
      tapInterval = clock_sum / num_of_taps;
      Serial.print("new tap Tempo is ");
      Serial.print(60000 / tapInterval);
      Serial.print(" bpm (");
      Serial.print(tapInterval);
      Serial.println(" ms interval)");

      // bpm = 60000 / tapInterval;
      tapState = 1;

      masterClock.begin(masterClockTimer, tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
    }

    if (timeSinceFirstTap > 2000) // forget tap if time was too long
    {
      tapState = 1;
      // Serial.println("too long...");
    }
    // }
    break;
  }
}
// --------------------------------------------------------------------
