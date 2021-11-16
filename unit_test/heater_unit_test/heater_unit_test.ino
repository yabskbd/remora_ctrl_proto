#define NUM_HEATERS 4
#define HEATERS_TO_TURN_ON_AT_ONCE 2
#define SLEEP_TIME_S 10


#define HEATER_CTRL_1 31
#define HEATER_CTRL_2 32

#define HEATER_CTRL_3 33
#define HEATER_CTRL_4 34

int all_heater [] = {HEATER_CTRL_1, HEATER_CTRL_2, HEATER_CTRL_3, HEATER_CTRL_4};

int set1_heater [HEATERS_TO_TURN_ON_AT_ONCE] = {HEATER_CTRL_1, HEATER_CTRL_4};

int set2_heater [HEATERS_TO_TURN_ON_AT_ONCE] = {HEATER_CTRL_2, HEATER_CTRL_3};

void setup() {
  Serial.begin(9600);
  // Configure all pins for output
  for (int i = 0; i < NUM_HEATERS; i++)
  {
    pinMode(all_heater[i], OUTPUT);
    digitalWrite(all_heater[i], LOW);
  }
}

void on_heater(int * heater_set)
{
	
	for (int i = 0; i < HEATERS_TO_TURN_ON_AT_ONCE; i++)
    {
      Serial.print("Turning on ");
      Serial.println(heater_set[i]);
      digitalWrite(heater_set[i], HIGH);
    }
}

void off_heater(int * heater_set)
{
	
	for (int i = 0; i < HEATERS_TO_TURN_ON_AT_ONCE; i++)
    {
      Serial.print("Turning OFF");
      Serial.println(heater_set[i]);
      digitalWrite(heater_set[i], LOW);
    }
}

void loop() {
	/*Turn Set 1 on */
	on_heater(set1_heater);
    delay(1000 * SLEEP_TIME_S);
	off_heater(set1_heater);
	delay(1000);
	on_heater(set2_heater);
	delay(1000 * SLEEP_TIME_S);
	off_heater(set2_heater);
	delay(1000);
}
