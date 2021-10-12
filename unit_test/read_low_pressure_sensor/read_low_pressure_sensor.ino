void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}



#define SENSOR_PIN A4
#define RMU_CTRL_DEFS_MAX_VOLTAGE 5
#define ADC_MAX_RESOLUTION_IN_BITS     10
#define ADC_MAX_RESOLUTION             ((1 << ADC_MAX_RESOLUTION_IN_BITS) - 1)
#define RMU_CTRL_SENSORS_EGP_MIN_VOLTAGE 0.5
#define RMU_CTRL_SENSORS_EGP_MAX_VOLTAGE 4.5
#define RMU_CTRL_SENSORS_EGP_ADC_RESOLUTION                                                                            \
  (double)(((RMU_CTRL_SENSORS_EGP_MAX_VOLTAGE - RMU_CTRL_SENSORS_EGP_MIN_VOLTAGE) / RMU_CTRL_DEFS_MAX_VOLTAGE) *       \
           ADC_MAX_RESOLUTION)
#define RMU_CTRL_SENSORS_EGP_ADC_VAL_OFFSET                                                                            \
  (double)((RMU_CTRL_SENSORS_EGP_MIN_VOLTAGE / RMU_CTRL_DEFS_MAX_VOLTAGE) * ADC_MAX_RESOLUTION)

double
rmu_utils_adc_range_based_interpreter(int adc_val, double adc_range, double min_range, double max_range, double offset)
{

  double total_range = max_range - min_range;
  double rescaled_adc_val = adc_val - offset;

  return ((((double)rescaled_adc_val / adc_range) * (total_range)) + min_range);
}



void loop() {
  // put your main code here, to run repeatedly:
   int adc_val = analogRead(SENSOR_PIN);
   Serial.println(rmu_utils_adc_range_based_interpreter(adc_val, RMU_CTRL_SENSORS_EGP_ADC_RESOLUTION, -14.7, 15,
                                                          RMU_CTRL_SENSORS_EGP_ADC_VAL_OFFSET));
}
