/****************************************************************************
 * 
 * Read in AIRFLOW SENSOR  
 * 
 *****************************************************************************/

/*! Configrable value starts */
/*! Air Flow Sensor Read in Value */
int analogPin = A5; 
#define AIR_FLOW_SENSOR_MAX_ACFM          25
/*! Configrable value ends */

/*! Raw Analog Value */                    
int val = 0;  
/*! Air Flow in ACFM Value*/   
int airflow = 0; 


/*! This is just the number of bits avaliable by arduino */   
#define AIR_FLOW_SENSOR_MAX_ADC           1023

void setup() {
  Serial.begin(9600);           //  setup serial
  Serial.print("Airflow is ACFM ");
}

void loop() {
  val = analogRead(analogPin);  // read the input pin
  airflow = (val*AIR_FLOW_SENSOR_MAX_ACFM)/AIR_FLOW_SENSOR_MAX_ADC;

  Serial.println(airflow);

  delay(5000);
}

