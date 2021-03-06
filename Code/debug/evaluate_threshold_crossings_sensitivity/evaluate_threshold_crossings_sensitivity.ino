/* teensyDrums00_Debug_pinReads_timer.ino
   for debugging the timer counts after instrument stroke.
   output can be nicely displayed by multiSensorLogging.py !
   github.com/dunland, August 2020

   0. get threshold-values from teensy_contactMic.ino
   1. set threshold[] values for all pins
   2. maybe set globalStrokeDelay

   program will print counted crossings for all pins
*/

IntervalTimer myTimer; // Create an IntervalTimer object

// ----------------------------------- input pins ------------------------------
static const uint8_t pins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int numInputs = 7;

const int ALL = 0; // prints all instruments crossings
const int CHANNEL = 1; // prints only selected channel's crossings
const int channel = 4; // and then decide which channel to sample...
const int MODE = CHANNEL; // choose whether to print all instruments or just one (channel)

const int ledPin = LED_BUILTIN;  // the pin with a LED

// ---------------- calibration- and sensitive-variant variables ---------------
// const int threshold[] = {30, 170, 170, 60}; // hihat, crash1, ride, Standtom
const int threshold[] = {180, 60, 300, 200, 400, 120, 80};
int noiseFloor[numInputs]; // to be set in setup
int min_crossings_for_signature[] = {1, 1, 1, 1, 1, 1, 1, 1}; // TODO: find characteristic signatures and insert here
int globalStrokeDelay = 2;

// ------------------ volatile variables forinterrupt timers ------------------
volatile int crossings[numInputs];
// volatile int vals[numInputs][1000];
// volatile boolean pinActive[numInputs];
volatile unsigned long lastPinActiveTime[numInputs];

/* -------------------------------------------------------------------------- */
/* ---------------------------------- SETUP --------------------------------- */
/* ------------------------------------------------------------------------ */

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  while (!Serial);

  // ----------------------------- calculate noiseFloor ------------------------
  // calculates the average noise floor out of 400 samples from all inputs

  int led_idx = 0;
  for (int pinNum = 0; pinNum < numInputs; pinNum++)
  {
    Serial.print("calculating noiseFloor for pin ");
    Serial.print(pins[pinNum]);
    // while(analogRead(pins[pinNum]) < threshold[pinNum]); // TODO: calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
    int totalSamples = 0;
    boolean toggleState = false;
    for (int n = 0; n < 200; n++)
    {
      //Serial.print(".");
      if (n % 50 == 0)
      {
        Serial.print(" . ");
        digitalWrite(ledPin, toggleState);
        toggleState = !toggleState;
      }
      totalSamples += analogRead(pins[pinNum]);
    }
    noiseFloor[pinNum] = totalSamples / 200;
    digitalWrite(ledPin, LOW);
    led_idx++;
    Serial.println(noiseFloor[pinNum]);
  }

  for (int i = 0; i < numInputs; i++) // turn LEDs off again
  {
    digitalWrite(ledPin, LOW);
    // noteSent[i] = false;
  }

  // ---------------------------- initialize arrays ----------------------------
  for (int i = 0; i < numInputs; i++)
  {
    //pinActive[i] = false;
    crossings[i] = 0;
  }
  Serial.print("millis");
  Serial.print("\t");
  Serial.print("pin");
  Serial.print("\t");
  Serial.print("cross");
  Serial.print("\t");
  Serial.print("thrshld");
  Serial.print("\t");
  Serial.println("noisFl");

  // ------------------------------ begin timer --------------------------------
  myTimer.begin(samplePin0, 1000);  // sample pin every 1 millisecond
}

/* --------------------------------------------------------------------- */
/* ------------------------------- TIMER ------------------------------- */
/* --------------------------------------------------------------------- */
void samplePin0()
// functions called by IntervalTimer should be short, run as quickly as
// possible, and should avoid calling other functions if possible.
{
  for (int pinNum = 0; pinNum < numInputs; pinNum++)
  {
    if (pinValue(pinNum) > threshold[pinNum])
    {
      lastPinActiveTime[pinNum] = millis();
      crossings[pinNum]++;
    }
  }
}

/* --------------------------------------------------------------------- */
/* --------------------------------- LOOP ------------------------------ */
/* --------------------------------------------------------------------- */

void loop() {
  static int crossingsCopy[numInputs];
  static unsigned long lastPinActiveTimeCopy[numInputs];
  static int lastValue[numInputs]; // for LED toggle
  static boolean toggleState = false; // for LED toggle
  // static int n = 0; // for loop debugging
  static unsigned long lastMillis;

  //if (millis() != lastMillis) Serial.println(millis());

  if (MODE == ALL)
  {
    for (int i = 0; i < numInputs; i++)
    {
      noInterrupts();
      lastPinActiveTimeCopy[i] = lastPinActiveTime[i];
      interrupts();

      if (millis() > lastPinActiveTimeCopy[i] + globalStrokeDelay) // get crossings only X ms after last hit
      {

        noInterrupts();
        crossingsCopy[i] = crossings[i];
        crossings[i] = 0;
        interrupts();

        if (crossingsCopy[i] >= min_crossings_for_signature[i])
        {
          //Serial.println(n++);
          if (crossingsCopy[i] != lastValue[i]) toggleState = !toggleState;
          digitalWrite(LED_BUILTIN, toggleState);
          Serial.print(millis());
          Serial.print("\t");
          //Serial.print(" pin ");
          Serial.print(pins[i]);
          Serial.print("\t");
          //Serial.print(": ");
          Serial.print(crossingsCopy[i]);
          //Serial.print(" zero-crossings. \t(");
          //Serial.print("threshold = ");
          Serial.print("\t");
          Serial.print(threshold[i]);
          Serial.print("\t");
          Serial.println(noiseFloor[i]);
          //Serial.println(")");
        }
        lastValue[i] = crossingsCopy[i];
      }
    }
  }

  else if (MODE == CHANNEL)
  {
    noInterrupts();
    lastPinActiveTimeCopy[channel] = lastPinActiveTime[channel];
    interrupts();

    if (millis() > lastPinActiveTimeCopy[channel] + globalStrokeDelay) // get crossings only X ms after last hit
    {

      noInterrupts();
      crossingsCopy[channel] = crossings[channel];
      crossings[channel] = 0;
      interrupts();

      if (crossingsCopy[channel] >= min_crossings_for_signature[channel])
      {
        //Serial.println(n++);
        if (crossingsCopy[channel] != lastValue[channel]) toggleState = !toggleState;
        digitalWrite(LED_BUILTIN, toggleState);
        Serial.print(millis());
        Serial.print("\t");
        //Serial.print(" pin ");
        Serial.print(pins[channel]);
        Serial.print("\t");
        //Serial.print(": ");
        Serial.print(crossingsCopy[channel]);
        //Serial.print(" zero-crossings. \t(");
        //Serial.print("threshold = ");
        Serial.print("\t");
        Serial.print(threshold[channel]);
        Serial.print("\t");
        Serial.println(noiseFloor[channel]);
        //Serial.println(")");
      }
      lastValue[channel] = crossingsCopy[channel];
    }
  }
  lastMillis = millis();

  //interrupts();
}

int pinValue(int pinNumber_in)
{
  int pinVal_temp = noiseFloor[pinNumber_in] - analogRead(pins[pinNumber_in]);
  pinVal_temp = abs(pinVal_temp); // positive values only
  return pinVal_temp;
}
