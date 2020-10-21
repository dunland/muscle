void setup() {
  Serial.begin(115200);
  while (!Serial);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  int n = 0;
  static boolean toggler = true;
  n = (n + 1) % 1000;

  if (millis() % 1000 == 0)
  {
    toggler = !toggler;
  }

  Serial.write(toggler);
    digitalWrite(LED_BUILTIN, toggler);
}
