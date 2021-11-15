#define NUM_HEATERS 4
#define HEATERS_TO_TURN_ON_AT_ONCE 2
#define SLEEP_TIME_S 10

void setup() {
  Serial.begin(9600);
  // Configure all pins for output
  for (int i = 0; i < NUM_HEATERS; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
}

void loop() {
  int idx = 0;
  while (true)
  {
    for (int i = 0; i < HEATERS_TO_TURN_ON_AT_ONCE; i++)
    {
      Serial.print("Turning on ");
      Serial.println(idx + i);
      digitalWrite(idx + i, HIGH);
    }

    delay(1000 * SLEEP_TIME_S);

    for (int i = 0; i < HEATERS_TO_TURN_ON_AT_ONCE; i++)
    {
      digitalWrite(idx + i, LOW);
    }

    idx = (idx + HEATERS_TO_TURN_ON_AT_ONCE) % NUM_HEATERS;
  }
}
