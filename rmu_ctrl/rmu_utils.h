#ifndef RMU_UTILS
#define RMU_UTILS

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
#define __ASSERT_USE_STDERR // Define ASSERT Internally 
#include <assert.h>
#include <SPI.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#define RMU_UTILS_EN_LP_PRINT
#define RMU_UTILS_EN_HP_PRINT



#define Sprintln_Output_Debug(a)            (Serial.println(a))


/*! Remove def for prodcution code 
    Todo define production #define */
#ifdef RMU_UTILS_EN_HP_PRINT
#define Sprintln_HP(a)            (Serial.println(a))
#define Sprint_HP(a)              (Serial.print(a))
#define Sprint_ext_HP(a, b)       (Serial.print(a, b))
#define Sprintln_ext_HP(a, b)     (Serial.println(a, b))
#else
#define Sprintln_HP(a)         
#define Sprint_HP(a)           
#define Sprint_ext_HP(a, b)    
#define Sprintln_ext_HP(a, b)  
#endif


#ifdef RMU_UTILS_EN_LP_PRINT
#define Sprintln(a)            (Serial.println(a))
#define Sprint(a)              (Serial.print(a))
#define Sprint_ext(a, b)       (Serial.print(a, b))
#define Sprintln_ext(a, b)     (Serial.println(a, b))

#define ASSERT(x)            assert(x)
#else /* Production Remove all Asserts and Prints */
#define Sprintln(a) 
#define Sprint(a)  
#define Sprint_ext(a, b)
#define Sprintln_ext(a, b) 

#define ASSERT(x)
#endif




/*! Assert Handler */
void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp);


/*! Input adc_raw_value and the Min & Max of the Range of the particular unit of interest. 
    Returns the value within range for the respective adc_val 
    Required: min_range < max_range */
double rmu_utils_adc_range_based_interpreter(int adc_val, double min_range, double max_range, double offset = 0);
  

#endif
