#include <SPI.h>
#include "Adafruit_MAX31855.h"

// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO   3
#define MAXCLK  4

#define NUM_MAXES 7

// initialize the Thermocouple
Adafruit_MAX31855* thermocouples;
void setup() {
  Serial.begin(9600);

  while (!Serial) delay(1); // wait for Serial on Leonardo/Zero, etc

  // wait for MAX chip to stabilize
  delay(500);
  thermocouples = (Adafruit_MAX31855*) malloc(sizeof(Adafruit_MAX31855) * NUM_MAXES);
  for (int i = 0; i < NUM_MAXES; i++)
  {
    int start_pin = i + 5;
    thermocouples[i] = Adafruit_MAX31855(MAXCLK, start_pin, MAXDO);
    if (!thermocouples[i].begin()) {
      while (1) delay(10);
    }
  }
}

void loop() {
  double temperatures[NUM_MAXES];
  for (int i = 0; i < NUM_MAXES; i++)
  {
    double c = thermocouples[i].readCelsius();
    temperatures[i] = c;
  }

  for (int i = 0; i < NUM_MAXES; i++)
  {
    Serial.print(temperatures[i]);
    if (i != NUM_MAXES - 1)
    {
      Serial.print(",");  
    }
  }
  Serial.println();
  
  delay(1000);
}
