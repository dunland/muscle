#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);
int pins[] = {A9, A3, A4, A8, A6, A7, A5};
int thresholds[] = {170, 80, 100, 40, 100, 100, 150};
int noiseFloor[sizeof(pins)];

void setup()
{
  Serial.begin(9600);

  // for (int i = 0; i < sizeof(pins); i++)
  // calculateNoiseFloor(pins[i]);
}

void loop()
{

  int val = analogRead(A7);
  Serial.print(val); // TOM1
  Serial.print("\t");
  Serial.print(val); // TOM1
  delay(10);
}
