// include the ArduinoTapTempo library
#include <ArduinoTapTempo.h>

// make an ArduinoTapTempo object
ArduinoTapTempo tapTempo;

boolean buttonDown;

/* ---------------------------------- CUSTOM PARAMETERS --------------------------------- */

const int noiseFloor = 260; // TODO: create function to assess noiseFloor upon startup
const int max_val = 762;
const int globalThreshold = 12;
static const uint8_t pins[] = {A0, A1, A2, A3};
static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN1, LED_BUILTIN2, LED_BUILTIN3};
static const uint8_t numInputs = 1; // that makes 4 input since number 0 is counted as well
int vals[numInputs];

void setup() {
  // begin serial so we can see the state of the tempo object through the serial monitor
  Serial.begin(19200);

  // set all required pins
  for (int i = 0; i < numInputs; i++)
  {
    pinMode(leds[i], OUTPUT);
    //pinMode(pins[i], INPUT);
  }

  tapTempo.setMaxBPM(330);
  tapTempo.setMinBPM(30);
}

void loop() {
  
  //Serial.println(analogRead(A0)); // debugging signal
  
  // get the state of the button
  for (int i = 0; i < numInputs; i++)
  {
    vals[i] = noiseFloor - analogRead(pins[i]);
    vals[i] = abs(vals[i]); // positive values only
    vals[i] = map(vals[i], 0, max_val, 0, 255); // 8-bit conversion
    if (vals[i] > globalThreshold)
    {
      digitalWrite(leds[i], HIGH);
      buttonDown = true;
    }
    else
    {
      digitalWrite(leds[i], LOW);
      buttonDown = false;
    }

  }
  
  // update ArduinoTapTempo
  tapTempo.update(buttonDown);


  Serial.print("bpm: ");
  Serial.println(tapTempo.getBPM());

  // uncomment the block below to demo many of ArduinoTapTempo's methods
  // note that Serial.print() is not a fast operation, and using it decreases the accuracy of the the tap timing
  uint64_t beat_interval = (60000 / tapTempo.getBPM());
  if (millis() % beat_interval > 100) 
  {
    digitalWrite(leds[0], HIGH);
  }
  else
  {
    digitalWrite(leds[0], LOW);
  }
  /*
    Serial.print("len:");
    Serial.print(tapTempo.getBeatLength());
    Serial.print(",\tbpm: ");
    Serial.print(tapTempo.getBPM());
    Serial.print(",\tchain active: ");
    Serial.print(tapTempo.isChainActive() ? "yes" : "no ");
    Serial.print(",\tlasttap: ");
    Serial.print(tapTempo.getLastTapTime());
    Serial.print(",\tprogress: ");
    Serial.print(tapTempo.beatProgress());
    Serial.print(",\tbeat: ");
    Serial.println(tapTempo.onBeat() ? "beat" : "    ");
  */
}
