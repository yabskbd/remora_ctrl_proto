#ifndef RMU_UTILS
#define RMU_UTILS

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


#undef RMU_UTILS_EN_LP_PRINT
#undef RMU_UTILS_EN_HP_PRINT



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
