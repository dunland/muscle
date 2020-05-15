const int TOLERANCE = 8;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(8, OUTPUT);
  pinMode(2, OUTPUT);
}

void loop() {

Serial.println(analogRead(A1));
/*
  if (abs(512 - analogRead(A0)) >= TOLERANCE)
  {
    Serial.println("TAP! A0");
    digitalWrite(8, HIGH);
    delay(50);
    digitalWrite(8, LOW);
  }
  if (abs(512 - analogRead(A1)) >= TOLERANCE)
  {
    Serial.println("TAP! A1");
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
  }*/
}
