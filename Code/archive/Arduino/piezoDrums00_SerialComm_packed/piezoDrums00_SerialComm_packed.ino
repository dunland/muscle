const int noiseFloor = 260; // TODO: create function to assess noiseFloor upon startup
const int max_val = 762;
const int globalThreshold = 32;
static const uint8_t pins[] = {A0, A1, A2, A3};
static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN1, LED_BUILTIN2, LED_BUILTIN3};
static const uint8_t numInputs = 2; // that makes 4 input since number 0 is counted as well
int vals[numInputs];

void setup() {
  Serial.begin(19200);

  // set all required pins
  for (int i = 0; i < numInputs; i++)
  {
    pinMode(leds[i], OUTPUT);
    //pinMode(pins[i], INPUT);
  }
}

void loop() {

  // Serial.println(analogRead(A0));

  // read all input pins
  for (int i = 0; i < numInputs; i++)
  {
    vals[i] = noiseFloor - analogRead(pins[i]);
    vals[i] = abs(vals[i]); // positive values only
    vals[i] = map(vals[i], 0, max_val, 0, 255); // 8-bit conversion
    //Serial.println(diff);
    //Serial.write(val);
    if (vals[i] > globalThreshold)
      //if (diff > 100)
    {
      digitalWrite(leds[i], HIGH);
      //Serial.write(vals[i]);
    }
    else
    {
      digitalWrite(leds[i], LOW);
    }
  }
  for (int i = 0; i < numInputs; i++)
  {
    Serial.write(vals[i]);
  }
  /* Serial.write anstatt print, da comport nur 8-bit codiert ist!
      in PD kommt es dann zum overflow und Werte um ~10 werden ausgegeben
      Serial.print gibt Werte in 10-bit aus
  */
  /*
     Serial.write(analogRead(A0));
     Serial.write(analogRead(A1));
     Serial.write(analogRead(A2));
     Serial.write(analogRead(A3));
  */

}
