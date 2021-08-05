
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


/*! Input adc_raw_value, the Min & Max of the Range of the particular unit of interest, and offset value to transform to Volts. 
    Returns the value within range for the respective adc_val 
    Required: min_range < max_range */
double rmu_utils_adc_range_based_interpreter(int adc_val, double min_range, double max_range, double offset = 0) {

  double total_range = max_range - min_range;
  ASSERT(max_range > min_range);

  double rescaled_adc_val = adc_val - offset;

  return ((((double)rescaled_adc_val/ADC_MAX_RESOULTION) * (total_range)) + min_range);

}
