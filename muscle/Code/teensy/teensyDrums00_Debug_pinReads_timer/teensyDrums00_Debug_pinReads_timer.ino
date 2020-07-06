// Create an IntervalTimer object
IntervalTimer myTimer;
// const int threshold[] = {30, 170, 170, 60}; // hihat, crash1, ride, Standtom
int threshold;
const int ledPin = LED_BUILTIN;  // the pin with a LED

static const uint8_t pins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int numInputs = 8;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  for (int i = 0; i<numInputs; i++)
  {
    crossings[i] = 0;
  }

  myTimer.begin(samplePin0, 1000);  // sample pin every 0.001 seconds
}

// The interrupt will blink the LED, and keep
// track of how many times it has blinked.
// int ledState = LOW;
// volatile unsigned long blinkCount = 0; // use volatile for shared variables
// volatile unsigned long x = 0;
// volatile int[] samples[100000];
volatile crossings[numInputs];
volatile totalSamples = 0;

void samplePin0()
{
  for (int pinNum = 0; pinNum < numInputs; pinNum++)
  {
  if (pinValue(pinNum) >= threshold)
  {
    corssings[pinNum]++;
  }
  // totalSamples++;
}
  // samples[x] = analogRead(samplePin);
  // x++;
}

// functions called by IntervalTimer should be short, run as quickly as
// possible, and should avoid calling other functions if possible.
// void blinkLED() {
//   if (ledState == LOW) {
//     ledState = HIGH;
//     blinkCount = blinkCount + 1;  // increase when LED turns on
//   } else {
//     ledState = LOW;
//   }
//   digitalWrite(ledPin, ledState);
// }

// The main program will print the blink count
// to the Arduino Serial Monitor
void loop() {
  // unsigned long blinkCopy;  // holds a copy of the blinkCount
  int crossingsCopy;
  int totalSamplesCopy;
  // to read a variable which the interrupt code writes, we
  // must temporarily disable interrupts, to be sure it will
  // not change while we are reading.  To minimize the time
  // with interrupts off, just quickly make a copy, and then
  // use the copy while allowing the interrupt to keep working.
  noInterrupts();
  // blinkCopy = blinkCount;
  // crossingsCopy = crossings;
  // totalSamplesCopy = totalSamples;
  interrupts();



  // if (xCopy >= 1000)
  // {
  //
  // noInterrupts();
  // String sampleString = "";
  // for (int smp = 0; smp<xCopy; smp++)
  // {
  //   sampleString += str(sample[x]);
  // }
  // x = 0;
  // interrupts();
  // }

  // Serial.print("blinkCount = ");
  // Serial.println(blinkCopy);
  // delay(100);
}

int pinValue(int pinNumber_in)
{
  int pinVal_temp = noiseFloor - analogRead(pinNumber_in);
  pinVal_temp = abs(pinVal_temp); // positive values only
  // pinVal_temp = map(pinVal_temp, 0, max_val, 0, 255); // 8-bit conversion
  return pinVal_temp;
}
