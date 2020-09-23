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
    if (use_responsiveCalibration)
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
boolean lastPinAction[numInputs];

void checkFootSwitch()
{

  static int switch_state;
  static int last_switch_state = HIGH;
  static unsigned long last_switch_toggle = 1000; // some pre-delay to prevent initial misdetection
  //static boolean lastPinAction[numInputs];

  switch_state = digitalRead(FOOTSWITCH);
  if (switch_state != last_switch_state && millis() > last_switch_toggle + 20)
  {
    if (switch_state == LOW)
    {
      footswitch_pressed();
      Serial.println("Footswitch pressed.");
    }
    else
    {
      footswitch_released();
      Serial.println("Footswitch released.");
    }
    last_switch_state = switch_state;
    last_switch_toggle = millis();
  }
}
// --------------------------------------------------------------------

int cc_val[numInputs];
boolean footswitch_is_pressed = false;

void footswitch_pressed()
{
  switch (FOOTSWITCH_MODE)
  {
  case (LOG_BEATS):
    // set pinMode of all instruments to 3 (record what is being played)
    for (int i = 0; i < numInputs; i++)
    {
      lastPinAction[i] = pinAction[i]; // TODO: lastPinAction seems to be overwritten quickly, so it will not be able to return to its former state. fix this!
      pinAction[i] = 3;                // TODO: not for Cowbell?
      for (int j = 0; j < 8; j++)
        set_rhythm_slot[i][j] = false; // reset entire record
    }
    break;

  case (HOLD_CC): // prevents cc_vals to be changed in swell_beat()
    footswitch_is_pressed = true;
    break;

  default:
    Serial.println("Footswitch mode not defined!");
    break;
  }
}
// --------------------------------------------------------------------

void footswitch_released()
{
  switch (FOOTSWITCH_MODE)
  {
  case (LOG_BEATS):
    for (int i = 0; i < numInputs; i++)
      pinAction[i] = initialPinAction[i];
    break;

  case (HOLD_CC):
    footswitch_is_pressed = false;
    break;

  default:
    break;
  }
}
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

// int cc_val[numInputs]; // this should be done in the swell section, but is needed in print section already... :/

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

    // case 5: // print cc_val for repeated MIDI notes in "swell" mode
    //   output_string[incoming_i] = cc_val[incoming_i];
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
  ------v++-----v++-----v++-----v++------ increase cc_val


  PLAY:

  -------|-------|-------|-------|-------
  -------0-------1-------2-------3------- beatStep
  -------|-------|-------|-------|-------
  -------v-----(v↓)----(v↓)----(v↓)------ decrease cc_val
  -------🎵-------🎵-------🎵------🎵------- play MIDI note

  ---------------------------------------------------------------------*/

int num_of_swell_taps[numInputs];     // will be used in both swell_rec() and swell_beat(). serves as cc_val for MIDI notes.
int swell_stroke_interval[numInputs]; // will be needed for timed replay
int swell_state[numInputs];
unsigned long swell_beatPos_sum[numInputs];
int swell_beatStep[numInputs]; // increases with beatCount and initiates action.

void swell_init() // initialize arrays for swell functions (run in setup())
{
  for (int i = 0; i < numInputs; i++)
  {
    swell_state[i] = 1; // waits for first tap
    num_of_swell_taps[i] = 0;
    cc_val[i] = 10;
    swell_beatPos_sum[i] = 0;
    swell_beatStep[i] = 0;
  }
}
// --------------------------------------------------------------------

// -------------------- SOUND SWELL: RECORD STROKES -------------------
// --------------------------------------------------------------------

void swell_rec(int instr) // remembers beat stroke position
{
  /* works pretty much just like the tapTempo, but repeats the triggered drums on external MIDI instrument (-> swell_beat() in TIMED INTERVALS) */
  static unsigned long previous_swell_beatPos[numInputs];
  static unsigned long lastSwellRec = 0;

  if (swell_state[instr] == 1) // first hit
  {
    // grab the current beat counter
    noInterrupts();
    previous_swell_beatPos[instr] = beatCount;
    interrupts();

    // start MIDI note and proceed to next state
    swell_state[instr] = 2;
    MIDI.sendNoteOn(notes_list[instr], 127, 2);

    lastSwellRec = millis();
  }

  else if (swell_state[instr] == 2) // second hit
  {
    if (!footswitch_is_pressed)
    {
      num_of_swell_taps[instr]++;
      cc_val[instr] += 2;                                          // ATTENTION: must rise faster than it decreases! otherwise swell resets right away.
      cc_val[instr] = (cc_val[instr] > 127) ? 127 : cc_val[instr]; // max cc_val = 127
    }

    unsigned long current_swell_beatPos;

    noInterrupts();
    current_swell_beatPos = beatCount;
    interrupts();

    // calculate diff and interval:
    if ((current_swell_beatPos - previous_swell_beatPos[instr]) <= 32) // only do this if interval was not too long
    {
      // get diff to first (how many beats were in between?):
      swell_beatPos_sum[instr] += (current_swell_beatPos - previous_swell_beatPos[instr]);
      // average all hits and repeat at that rate:
      float a = float(swell_beatPos_sum[instr]) / float(num_of_swell_taps[instr]);
      a += 0.5;                              //rounds up or down
      swell_stroke_interval[instr] = int(a); // round down
      previous_swell_beatPos[instr] = current_swell_beatPos;
    }
  }
}
// --------------------------------------------------------------------

// -------------------- SOUND SWELL: REPLAY STROKES -------------------
// --------------------------------------------------------------------

void swell_beat(int instr) // updates once a 32nd-beat-step
{
  if (swell_state[instr] == 2)
  {
    // remember update moment in beat:
    noInterrupts();
    unsigned long updateMoment = beatCount;
    interrupts();

    // ready to play MIDI again?
    // increase swell_beatStep and modulo interval
    swell_beatStep[instr] = (swell_beatStep[instr] + 1) % swell_stroke_interval[instr];

    if (swell_beatStep[instr] == 0) // on swell beat
    {
      //output_string[instr] = String(cc_val[instr]);
      //      output_string[instr] = "[";
      //      output_string[instr] += String(swell_stroke_interval[instr]);
      //      output_string[instr] += "] ";
      //      output_string[instr] += num_of_swell_taps[instr];
      //      output_string[instr] += "/";
      //      output_string[instr] += swell_beatPos_sum[instr];
      //      output_string[instr] += " (";
      output_string[instr] = String(cc_val[instr]);
      //      output_string[instr] += ") ";
      output_string[instr] += "\t";
      if (!footswitch_is_pressed)
        MIDI.sendControlChange(cc_chan[instr], cc_val[instr], 2); //
      /* channels on mKORG: 44=cutoff, 50=amplevel, 23=attack, 25=sustain, 26=release
        finding the right CC# on microKORG: (manual p.61):
        1. press SHIFT + 5
        2. choose parameter to find out via EDIT SELECT 1 & 2
        (3. reset that parameter, if you like) */
      // MIDI.sendNoteOn(notes_list[instr], 127, 2);

      // decrease cc_val:
      if (cc_val[instr] > 0)
      {
        //static boolean cc_val_decreaser = true;
        //if (cc_val_decreaser)
        //{
        if (!footswitch_is_pressed)
          cc_val[instr]--;
        //}
        //cc_val_decreaser = !cc_val_decreaser; // decrease only every other time
      }
      else // reset swell if cc_val == 0:
      {
        swell_state[instr] = 1; // waits for first tap
        num_of_swell_taps[instr] = 0;
        cc_val[instr] = 10;
        swell_beatPos_sum[instr] = 0;
        swell_beatStep[instr] = 0;
        MIDI.sendNoteOff(notes_list[instr], 127, 2);
      } //TODO: reset!
      // cc_val[instr] = (cc_val[instr] > 0) * (cc_val[instr] - 1); // decrease cc_val only if > 0
    }
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

//////////////////// TSUNAMI BEAT-LINKED PLAYBACK /////////////////////
///////////////////////////////////////////////////////////////////////

/* ------------- Tsunami Beat-linked Playback algorithm: --------------


 X-----------X---X--------------- 32nd Beat
[1,  0,  0,  1,  1,  0,  0,  0  ] 1. iteration
 |-----------|---|---------------
+1, +0, +0, +1, +1, +0, +0, +0    changes between iterations
 |-----------|---|---------------
[2,  0,  0,  2,  2,  0,  0,  0  ] 2. iteration
 |-----------|---|---------------
+0. +1, +0, +1, +1, +0, +0, +0    changes, unprecisely played
 |-----------|---|---------------
[2,  1,  0,  3,  3,  0,  0,  0  ] 3. iteration
 |-----------|---|---------------
 4 + 1 + 0 + 9 + 9 + 0 + 0 + 0 = 23 beat_topo_squared_sum
 |-----------|---|---------------

4/23 = 0.174
          ratio = 4:1 = 0.25
1/23 = 0.043
          ratio = 9:1 = 0.111 --> beat_topo_entries -= 1
9/23 = 0.391

beat_topo_regular_sum = 2 + 1 + 3 + 3 = 9
beat_topo_entries = 4 - 1 = 3
beat_topo_average = int(9/3 + 0.5) = 3

---------------------------------------------------------------------*/

void tsunami_beat_playback(int instr)
{
  int beat_topo_entries = 0;
  int beat_topo_squared_sum = 0;

  // count entries and create squared sum:
  for (int j = 0; j < 8; j++)
  {
    if (beat_topography[instr][j] > 0)
    {
      beat_topo_entries++;
      beat_topo_squared_sum += beat_topography[instr][j] * beat_topography[instr][j];
    }
  }

  // calculate site-specific fractions (squared):
  float beat_topo_squared_frac[8];
  for (int j = 0; j < 8; j++)
    beat_topo_squared_frac[j] =
        float(beat_topography[instr][j]) / float(beat_topo_squared_sum);

  // get highest frac:
  float highest_frac = 0;
  for (int j = 0; j < 8; j++)
    highest_frac = (beat_topo_squared_frac[j] > highest_frac) ? beat_topo_squared_frac[j] : highest_frac;

  // get "topography height":
  // divide highest with other entries and omit entries if ratio > 3:
  for (int j = 0; j < 8; j++)
    if (highest_frac / beat_topo_squared_frac[j] > 3 || beat_topo_squared_frac[j] / highest_frac > 3)
      beat_topo_entries -= 1;

  // assess average topo sum for loudness
  int beat_topo_regular_sum = 0;
  for (int j = 0; j < 8; j++)
    beat_topo_regular_sum += beat_topography[instr][j];
  int beat_topo_average = int(float(beat_topo_regular_sum) / float(beat_topo_entries) + 0.5);

  // TODO:
  // loudness will be linked to the beat_topo_average
  // set gain and play track from tsunami
}

/////////////////////////// DEBUG FUNCTIONS ///////////////////////////
///////////////////////////////////////////////////////////////////////

// print the play log to Arduino console:
void print_to_console(String message_to_print)
{
  if (do_print_to_console)
    Serial.print(message_to_print);
}

void println_to_console(String message_to_print)
{
  if (do_print_to_console)
    Serial.println(message_to_print);
}

// or send stuff to processing:
void send_to_processing(int message_to_send)
{
  if (do_send_to_processing)
    Serial.write(message_to_send);
}
