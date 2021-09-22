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

/*! Humidity MV Range Scaling */
#define MAX_MV_HUM_READ      5000.0
#define MIN_MV_HUM_READ      0

#define RMU_CTRL_SENSORS_EGH_SLOPE  0.03892
#define RMU_CTRL_SENSORS_EGH_OFFSET -42.017

#define MAX_NUM_HUM_SENSORS          3

int hum_sensor_adc_pin [MAX_NUM_HUM_SENSORS] = {A0, A1, A2};

/*! Input adc_raw_value, the Min & Max of the Range of the particular unit of interest, and offset value to transform to
   Volts. Returns the value within range for the respective adc_val Required: min_range < max_range */
double rmu_utils_adc_range_based_interpreter(int adc_val, double min_range, double max_range, double offset)
{

  double total_range = max_range - min_range;
  double rescaled_adc_val = adc_val - offset;

  return ((((double)rescaled_adc_val / ADC_MAX_RESOULTION) * (total_range)) + min_range);
}



void setup() {
  // declare the ledPin as an OUTPUT:
  //pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // read the value from the sensor:
  for (int idx = 0; idx < MAX_NUM_HUM_SENSORS; idx++)
  {
    int sensorPin = hum_sensor_adc_pin[idx];
    int adc_val = analogRead(sensorPin);
    double mv_out = rmu_utils_adc_range_based_interpreter(adc_val, MIN_MV_HUM_READ, MAX_MV_HUM_READ, 0);
    double relative_humidity = RMU_CTRL_SENSORS_EGH_SLOPE * mv_out + RMU_CTRL_SENSORS_EGH_OFFSET;

    Serial.print(relative_humidity);
    Serial.print(",");
    delay(500);    
  }

  Serial.println();
  delay(3000);

}
