/*
  Analog Input

  Demonstrates analog input by reading an analog sensor on analog pin 0 and
  turning on and off a light emitting diode(LED) connected to digital pin 13.
  The amount of time the LED will be on and off depends on the value obtained
  by analogRead().

  The circuit:
  - potentiometer
    center pin of the potentiometer to the analog input 0
    one side pin (either one) to ground
    the other side pin to +5V
  - LED
    anode (long leg) attached to digital output 13 through 220 ohm resistor
    cathode (short leg) attached to ground

  - Note: because most Arduinos have a built-in LED attached to pin 13 on the
    board, the LED is optional.

  created by David Cuartielles
  modified 30 Aug 2011
  By Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInput
*/
#include <assert.h>

#define ASSERT(x) assert(x)
/*! ADC Max Resolution in bits
    For Arduino ref: https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/ 
    Mega Resolution is 10bits i.e 0 - 1023 */
#define ADC_MAX_RESOLUTION_IN_BITS     10
#define ADC_MAX_RESOULTION             ((1 << ADC_MAX_RESOLUTION_IN_BITS) - 1)

/*! Temprature Range Scaling */
#define MAX_TEMP_C_DEGREE      500.0
#define MIN_TEMP_C_DEGREE      0


int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor

/*! Input adc_raw_value and the Min & Max of the Range of the particular unit of interest. 
    Returns the value within range for the respective adc_val 
    Required: min_range < max_range */
double adc_range_based_interpreter(int adc_val, double min_range, double max_range) {
  
  double total_range = max_range - min_range;
  ASSERT(max_range > min_range);
  return ((((double)adc_val/ADC_MAX_RESOULTION) * (total_range)) + min_range);
}

void setup() {
  // declare the ledPin as an OUTPUT:
  //pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // read the value from the sensor:
  int adc_val = analogRead(sensorPin);
  Serial.print("ADC Raw Value ");
  Serial.println(adc_val);
  
  double temp_in_c = adc_range_based_interpreter(adc_val, MIN_TEMP_C_DEGREE, MAX_TEMP_C_DEGREE);
  Serial.print("Thermo ");
  Serial.println(temp_in_c); 

  
  delay(3000);

}
