IntervalTimer myTimer; // Create an IntervalTimer object

// ----------------------------------- input pins ------------------------------
static const uint8_t pins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int numInputs = 4;

const int ledPin = LED_BUILTIN;  // the pin with a LED

// ---------------- calibration- and sensitive-variant variables ---------------
// const int threshold[] = {30, 170, 170, 60}; // hihat, crash1, ride, Standtom
const int threshold[] = {30, 60, 90, 120, 90, 110, 130};
int noiseFloor; // to be set in setup
int min_crossings_for_signature[] = {1, 1, 1, 1, 1, 1, 1, 1}; // TODO: find characteristic signatures and insert here

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
  Serial.begin(9600);
  while (!Serial);

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

  // ---------------------------- initialize arrays ----------------------------
  for (int i = 0; i < numInputs; i++)
  {
    //pinActive[i] = false;
    crossings[i] = 0;
  }

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
  for (int pinNum = 0; pinNum < numInputs; pinNum++) // TODO: must pinNum also be volatile?
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

  for (int i = 0; i < numInputs; i++)
  {
    noInterrupts();
    lastPinActiveTimeCopy[i] = lastPinActiveTime[i];
    interrupts();

    if (millis() > lastPinActiveTimeCopy[i] + 50) // get crossings only 50 ms after last hit
    {

      noInterrupts();
      crossingsCopy[i] = crossings[i];
      crossings[i] = 0;
      interrupts();

      if (crossingsCopy[i] >= min_crossings_for_signature[i])
      {
        //Serial.println(n++);
        if (crossingsCopy[i] != lastValue) toggleState = !toggleState;
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
        Serial.println(threshold[i]);
        //Serial.println(")");
      }
      lastValue[i] = crossingsCopy[i];
    }
  }
  //interrupts();
}

int pinValue(int pinNumber_in)
{
  int pinVal_temp = noiseFloor - analogRead(pinNumber_in);
  pinVal_temp = abs(pinVal_temp); // positive values only
  return pinVal_temp;
}
