/* --------------------------------------------------------------------- */
/* ----------------------------- FUNCTIONS ----------------------------- */
/* --------------------------------------------------------------------- */

// --------------------------- CALCULATE NOISEFLOOR -------------------
// --------------------------------------------------------------------
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
      while (analogRead(pins[pinNum]) < 700 + calibration[pinNum][0]); // calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
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


// ------------------------------- FOOT SWITCH ------------------------
// --------------------------------------------------------------------
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
        lastPinAction[i] = pinAction[i];
        pinAction[i] = 3; // TODO: not for Cowbell?
        //for (int j = 0; j < 8; j++) set_rhythm_slot[i][j] = false;
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


// ------------------------ general pin reading -----------------------
// --------------------------------------------------------------------

int pinValue(int pinVal_pointer_in)
{
  return abs(noiseFloor[pinVal_pointer_in] - analogRead(pins[pinVal_pointer_in]));
}
// --------------------------------------------------------------------

// ------------------------- PRINT NORMALIZED VALUES ------------------
// --------------------------------------------------------------------
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


// ------------------ SET STRING FOR PLAY LOGGING ---------------------
// --------------------------------------------------------------------
void setInstrumentPrintString(int incoming_i)
{
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
}
// --------------------------------------------------------------------


// ------------------------- STROKE DETECTION -------------------------
// --------------------------------------------------------------------
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
    // if (millis() > firstPinActiveTimeCopy[pinDect_pointer_in] + globalDelayAfterStroke) // get counts only X ms after FIRST hit ??
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


// ------------------------------ TAP TEMPO ---------------------------
// --------------------------------------------------------------------

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

    case 1:                                     // waiting for first hit
      if (millis() > timeSinceFirstTap + 10000) // reinitiate tap if not used for ten seconds
      {
        num_of_taps = 0;
        clock_sum = 0;
        Serial.println("-----------TAP RESET!-----------\n");
      }
      timeSinceFirstTap = millis(); // record time of first hit
      tapState = 2;                 // next: wait for second hit

      break;

    case 2: // waiting for second hit

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
