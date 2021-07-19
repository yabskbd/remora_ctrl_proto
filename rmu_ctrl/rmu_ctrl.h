#ifndef RMU_CTRL
#define RMU_CTRL

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





#endif
