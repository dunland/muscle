static const uint8_t pins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int numInputs = 8;
int noiseFloor;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  // --------------------------------------- calculate noiseFloor
  int pinNum = 0;
  int totalSamples = 0;
  int led_idx = 0;
  for (int n = 0; n < 400; n++)
  {
    // Serial.print(".");
    if (n % 100 == 0)
    {
      // Serial.println("");
      digitalWrite(LED_BUILTIN, HIGH);
      led_idx++;
    }
    pinNum = (pinNum + 1) % numInputs;
    totalSamples += analogRead(pins[pinNum]);
  }

  noiseFloor = totalSamples / 400;
  digitalWrite(LED_BUILTIN, LOW);

}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long startMeasuring = micros();
  volatile unsigned int totalMeasurements = 0;
  volatile int val;

  digitalWrite(LED_BUILTIN, HIGH);

  while (micros() < startMeasuring + 1000) // 10 ms of sampling time
  {
    for (int pinNum = 0; pinNum < numInputs; pinNum++)
    {
      val = pinValue(pinNum);
      totalMeasurements++;
    }
  }
  Serial.println(totalMeasurements);
  Serial.println(" taken in 1000 microseconds");
  digitalWrite(LED_BUILTIN, LOW);
  delay(10);
}

int pinValue(int pinNumber_in)
{
  int pinVal_temp = noiseFloor - analogRead(pinNumber_in);
  pinVal_temp = abs(pinVal_temp); // positive values only
  return pinVal_temp;
}
