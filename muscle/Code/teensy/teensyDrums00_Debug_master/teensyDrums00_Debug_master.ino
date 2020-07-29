unsigned long lastBlink;
int input_pin = A0;
int control_pin = A1; // best to use HiHat or other non-resposnsive instrument
int noiseFloor = 695;
int threshold = 0;

IntervalTimer myTimer; // Create an IntervalTimer object
int globalStrokeDelay = 20;
volatile int crossings;
volatile unsigned long lastPinActiveTime;

void setup() {
  Serial.begin(34800);
  while (!Serial);
  pinMode(input_pin, INPUT);
  pinMode(control_pin, INPUT);
  pinMode(13, OUTPUT);

}

/* --------------------------------------------------------------------- */
/* ------------------------------- TIMER ------------------------------- */
/* --------------------------------------------------------------------- */
void samplePin0()
// functions called by IntervalTimer should be short, run as quickly as
// possible, and should avoid calling other functions if possible.
{
  if (pinValNormalized(input_pin) > threshold)
  {
    lastPinActiveTime = millis();
    crossings++;
  }
}

void loop() {

  static int state = 0; // switch cannot be other than 0 or 1??

  //switch (state)
  //{
  //case 0:
  if (state == 0)
  {
    static unsigned long start = millis();
    while (millis() < start + 10000)
    {
      Serial.println(analogRead(input_pin));
      Serial.println(analogRead(control_pin));
    }
    state++;
    Serial.print("ENTERING STATE ");
    Serial.println(state);

    //break;
  }

  // ---------------------- print raw sensor data ---------------------
  // ------------------------------------------------------------------
  //case 1:
  else if (state == 1)
  {
    int pinValue = analogRead(input_pin);
    Serial.println(pinValue);

    if (pinValue > 500) // 500 is best guess to trigger something
    {
      digitalWrite(13, HIGH);
      lastBlink = millis();
    }
    else //if (millis() > lastBlink + 50)
    {
      digitalWrite(13, LOW);
    }
    delay(10); // Teensy Serial object always communicates at 12 Mbit/sec USB speed. Many computers can not update the serial monitor window if there is no delay to limit the speed!

    if (analogRead(control_pin) > 700)
    {
      delay(500); // make sure there is no oscillation on drum anymore..
      //calculateNoiseFloor();
      state = 2;
      Serial.print("ENTERING STATE ");
      Serial.println(state);
    }
    //break;
  }
  // --------------------- print normalized pin value ---------------
  // ----------------------------------------------------------------
  //case 2:
  else if (state == 2)
  {
    //Serial.println(millis());


    int pinValue = pinValNormalized(input_pin);
    Serial.println(pinValue);

    if (pinValue > 500)
    {
      digitalWrite(13, HIGH);
      //lastBlink = millis();
    }
    else //if (millis() > lastBlink + 50)
    {
      digitalWrite(13, LOW);
    }
    delay(10); // Teensy Serial object always communicates at 12 Mbit/sec USB speed. Many computers can not update the serial monitor window if there is no delay to limit the speed!

    // -------- set threshold via Console and go to next state ----------
          if (Serial.available() > 0)
          {
            threshold = Serial.read();
            Serial.print("threhsold set to ");
            Serial.println(threshold);
    
            myTimer.begin(samplePin0, 1000);  // sample pin every 1 millisecond
            state++;
          }

    //break;
  }
  // ------------ print crossings, vary globalStrokeDelay -----------
  // ----------------------------------------------------------------
  //case 3:
  else if (state == 3)
  {
    static int crossingsCopy;
    static unsigned long lastPinActiveTimeCopy;
    static int lastValue; // for LED toggle
    static boolean toggleState = false; // for LED toggle
    static unsigned long lastMillis;
    static int min_crossings_for_signature = 1;

    //   for (int i = 0; i < numInputs; i++)
    //{
    noInterrupts();
    lastPinActiveTimeCopy = lastPinActiveTime;
    interrupts();

    if (millis() > lastPinActiveTimeCopy + globalStrokeDelay) // get crossings only X ms after last hit
    {

      noInterrupts();
      crossingsCopy = crossings;
      crossings = 0;
      interrupts();

      if (crossingsCopy >= min_crossings_for_signature)
      {
        //Serial.println(n++);
        if (crossingsCopy != lastValue) toggleState = !toggleState;
        digitalWrite(LED_BUILTIN, toggleState);
        Serial.print(millis());
        Serial.print("\t");
        //Serial.print(" pin ");
        // Serial.print(input_pin);
        // Serial.print("\t");
        //Serial.print(": ");
        Serial.print(crossingsCopy);
        //Serial.print(" zero-crossings. \t(");
        //Serial.print("threshold = ");
        Serial.print("\t");
        Serial.print(threshold);
        Serial.print("\t");
        Serial.println(noiseFloor);
        //Serial.println(")");
      }
      lastValue = crossingsCopy;
    }
    //}
    lastMillis = millis();

    // TODO: vary globalStrokeDelay and set it up
    // break;
  }
  //default:
  //Serial.println(millis());
  //  break;
  //}
}

/* --------------------------------------------------------------------- */
/* ----------------------------- FUNCTIONS ----------------------------- */
/* --------------------------------------------------------------------- */

int pinValNormalized(int read_pin)
{
  return abs(noiseFloor - analogRead(read_pin));
}

void calculateNoiseFloor()
{
  // ----------------------------- calculate noiseFloor ------------------------
  // calculates the average noise floor out of 400 samples from all inputs

  //int led_idx = 0;
  //for (int pinNum = 0; pinNum < numInputs; pinNum++)
  //{
  //Serial.print("calculating noiseFloor for pin ");
  //Serial.print(input_pin);
  //Serial.print(" ..waiting for stroke");
  //if (responsiveCalibration)
  //{
  // while (analogRead(input_pin) < 500 + threshold[pinNum]); // calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
  //Serial.print(" .");
  //delay(1000); // should be long enough for drum not to oscillate anymore
  //}

  Serial.print("calculating noiseFloor...");

  int totalSamples = 0;
  boolean toggleState = false;
  for (int n = 0; n < 400; n++)
  {
    //Serial.print(".");
    if (n % 100 == 0)
    {
      Serial.print(" . ");
      digitalWrite(13, toggleState);
      toggleState = !toggleState;
    }
    totalSamples += analogRead(input_pin);
  }
  noiseFloor = totalSamples / 400;
  digitalWrite(13, LOW);
  //  led_idx++;
  Serial.print(" = ");
  Serial.println(noiseFloor);
  //}

  //for (int i = 0; i < numInputs; i++) // turn LEDs off again
  //{
  digitalWrite(13, LOW);
  //}
}
