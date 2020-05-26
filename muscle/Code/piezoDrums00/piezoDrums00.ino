const int noiseFloor = 260; // TODO: create function to assess noiseFloor upon startup
const int max_val = 762;
const int globalThreshold = 32;
static const uint8_t pins[] = {A0, A1, A2, A3};
static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN1, LED_BUILTIN2, LED_BUILTIN3};
static const uint8_t inputAmount = 3; // that makes 4 input since number 0 is counted as well

void setup() {
  Serial.begin(19200);

  // set all required pins
  for (int i = 0; i < inputAmount; i++)
  {
    pinMode(leds[i], OUTPUT);
    pinMode(pins[i], INPUT);
  }
}

void loop() {

  // Serial.println(analogRead(A0));

  // read all input pins
  //for (int i = 0; i < 3; i++)
  //{
    int diff0 = noiseFloor - analogRead(A0);
    diff0 = abs(diff0); // positive values only
    diff0 = map(diff0, 0, max_val, 0, 255);  // 8-bit conversion
    //Serial.println(diff);
    Serial.write(diff0);

    int diff1 = noiseFloor - analogRead(A1);
    diff1 = abs(diff1); // positive values only
    diff1 = map(diff1, 0, max_val, 0, 255);  // 8-bit conversion
    //Serial.println(diff);
    Serial.write(diff1);

    int diff2 = noiseFloor - analogRead(A2);
    diff2 = abs(diff2); // positive values only
    diff2 = map(diff2, 0, max_val, 0, 255);  // 8-bit conversion
    //Serial.println(diff);
    Serial.write(diff2);

    int diff3 = noiseFloor - analogRead(A3);
    diff3 = abs(diff3); // positive values only
    diff3 = map(diff3, 0, max_val, 0, 255);  // 8-bit conversion
    //Serial.println(diff);
    Serial.write(diff3);

    if (diff0 > globalThreshold)
      //if (diff > 100)
    {
      digitalWrite(leds[0], HIGH);
    }
    else
    {
      digitalWrite(leds[0], LOW);
    }

    if (diff1 > globalThreshold)
      //if (diff > 100)
    {
      digitalWrite(leds[1], HIGH);
    }
    else
    {
      digitalWrite(leds[1], LOW);
    }

    if (diff2 > globalThreshold)
      //if (diff > 100)
    {
      digitalWrite(leds[2], HIGH);
    }
    else
    {
      digitalWrite(leds[2], LOW);
    }

    if (diff3 > globalThreshold)
      //if (diff > 100)
    {
      digitalWrite(leds[3], HIGH);
    }
    else
    {
      digitalWrite(leds[3], LOW);
    }
  //}



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
