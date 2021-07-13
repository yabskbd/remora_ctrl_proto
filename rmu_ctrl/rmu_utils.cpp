//////////////////////////////////////////////////////////////////////////////////////
//NOTES
//////////////////////////////////////////////////////////////////////////////////////
//this code is meant to run the prototypes Fingerling era prototypes
//it is a reformatted version of DEMO_RF_TO_DATA_LOGGER
//this code should be modified on the "a" page using the "public" functions listed under the DataLogger class
  //such changes could be the number of sensors
  //startup for CO2 sensor
  //starting phase
//the "b" (and subsequent) page(s) should be used by someone familiar with the code to implement and/or adjust the code for different scenarios
//the DataLogger class contains all the sensors and the RTC
//the "a" page contains the SD card reader/writer, the fan, and the heater
//SHT20 is implemented poorly; if redoing, consider each output its own sensor
//bug in choosing to preheat or not
//dew point calculation from here https://iridl.ldeo.columbia.edu/dochelp/QA/Basic/dewpoint.html

//I am also considering adding a class on the third page to manage the DataLogger and switch phases

//b_DATA_LOG... contains all a class that runs all the sensors and compiles all the data to be sent to radio and SD card

//////////////////////////////////////////////////////////////////////////////////////
//LIBRARIES
//////////////////////////////////////////////////////////////////////////////////////

#include "rmu_utils.h"
#include "rmu_ctrl_defs.h"

/* -------------------------------------------------------------------------------------------------------------------*/
/*! Assert Handler */
void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp)
{
    // transmit diagnostic informations through serial link.
    Sprint("--------------------------------ERROR ASSERT------------------------");
    Sprint(__file);
    Sprint(__func);
    Serial.print("Line Number: ");
    Sprint_ext(__lineno, DEC);
    Sprint(__sexp);
    Serial.flush();
    // abort program execution.
    abort();
}


/*! Input adc_raw_value and the Min & Max of the Range of the particular unit of interest. 
    Returns the value within range for the respective adc_val 
    Required: min_range < max_range */
double adc_range_based_interpreter(int adc_val, double min_range, double max_range) {

  double total_range = max_range - min_range;
  ASSERT(max_range > min_range);
  return ((((double)adc_val/ADC_MAX_RESOULTION) * (total_range)) + min_range);

}
