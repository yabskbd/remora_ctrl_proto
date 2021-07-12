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
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>


#include "Adafruit_MAX31855.h"
#include "uFire_SHT20.h"
#include "SHT1x.h"
#include "mcp2515_can.h"
#define __ASSERT_USE_STDERR // Define ASSERT Internally 
#include <assert.h>


/*! Remove def for prodcution code 
    Todo define production #define */

#define Sprintln_HIGH(a)          (Serial.println(a))
#define Sprint_HIGH(a)            (Serial.print(a))
#define Sprint_ext_HIGH(a, b)     (Serial.print(a, b))



#if 0
#define Sprintln(a)          (Serial.println(a))
#define Sprint(a)            (Serial.print(a))
#define Sprint_ext(a, b)     (Serial.print(a, b))

#define ASSERT(x)            assert(x)

#else

#define Sprintln(a) 
#define Sprint(a)  
#define Sprint_ext(a, b)

#define ASSERT(x)

#endif
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



/* --------------------------------------------------CAN BUS----------------------------------------------------------*/

/*! CAN Bus Definitions */
// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
#define SPI_CS_PIN            9
#define CAN_BAUD_RATE_0       CAN_500KBPS
mcp2515_can CAN(SPI_CS_PIN);


/* EGT CAN ID Def. */ 
#define EGT_CAN_ID_0_1             0x100
#define EGT_CAN_ID_2_3             0x101
#define EGT_CAN_ID_4_5             0x102
#define EGT_CAN_ID_6_7             0x103

#define EGT_CAN_ID_8_9             0x104
#define EGT_CAN_ID_10_11           0x105
#define EGT_CAN_ID_12_13           0x106
#define EGT_CAN_ID_14_15           0x107


#define EGT_CAN_ID_0             EGT_CAN_ID_0_1
#define EGT_CAN_ID_1             EGT_CAN_ID_0_1
#define EGT_CAN_ID_2             EGT_CAN_ID_2_3
#define EGT_CAN_ID_3             EGT_CAN_ID_2_3
#define EGT_CAN_ID_4             EGT_CAN_ID_4_5
#define EGT_CAN_ID_5             EGT_CAN_ID_4_5
#define EGT_CAN_ID_6             EGT_CAN_ID_6_7
#define EGT_CAN_ID_7             EGT_CAN_ID_6_7
#define EGT_CAN_ID_8             EGT_CAN_ID_8_9
#define EGT_CAN_ID_9             EGT_CAN_ID_8_9
#define EGT_CAN_ID_10            EGT_CAN_ID_10_11
#define EGT_CAN_ID_11            EGT_CAN_ID_10_11
#define EGT_CAN_ID_12            EGT_CAN_ID_12_13
#define EGT_CAN_ID_13            EGT_CAN_ID_12_13
#define EGT_CAN_ID_14            EGT_CAN_ID_14_15
#define EGT_CAN_ID_15            EGT_CAN_ID_14_15


/* --------------------------------------------------Sensor Max Value Definitions----------------------------------------------------------*/

/*! Max Definitions */
/*! MAX Sensor Counts */
#define MAX_EGT_SENSORS                8
#define MAX_PR0_30_SENSORS             5
#define MAX_PR0_2K_SENSORS             0

#define MAX_SHT10_SENSORS              0
#define MAX_SHT20_SENSORS              0

/* -------------------------------------------------------------------------------------------------------------------*/
/*! Pin Mappings */
/* Thermocouple Serial Pins for CLK, CS, DO */
#define EGT_CLK_0       8
#define EGT_DO_0        9

/* Update to use new GPIO Pins */
#define EGT_CLK_1       8
#define EGT_DO_1        9


#define EGT_CS_0             10
#define EGT_CS_1             11
#define EGT_CS_2             12
#define EGT_CS_3             13
#define EGT_CS_4             22
#define EGT_CS_5             23
#define EGT_CS_6             24
#define EGT_CS_7             25
#define EGT_CS_8             9


/* Humidity Sensor SH1x CLK and DO Pin (PWM) */
#define SH1X_CLK_0            2
#define SH1X_DO_0             3

#define SH1X_CLK_1            4
#define SH1X_DO_1             5

#define SH1X_CLK_2            6
#define SH1X_DO_2             7

/* -------------------------------------------------------------------------------------------------------------------*/

/*! ADC Max Resolution in bits
    For Arduino ref: https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/ 
    Mega Resolution is 10bits i.e 0 - 1023 */
#define ADC_MAX_RESOLUTION_IN_BITS     10
#define ADC_MAX_RESOULTION             ((1 << ADC_MAX_RESOLUTION_IN_BITS) - 1)

/*! Temprature Range Scaling */
#define MAX_TEMP_C_DEGREE      500
#define MIN_TEMP_C_DEGREE      0

/*! Max Sensor Definitions */
#define SENSOR_SWEEP_INTERVAL_IN_MS    100
#define MAX_NUM_TEMP 11
#define MAX_NUM_PSI   5
#define DATA_LOG_BUF_SIZE (MAX_TEMP + MAX_PSI)

//////////////////////////////////////////////////////////////////////////////////////
//Variables
//////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  double data[MAX_EGT_SENSORS];
  
}rmu_ctrl_sensors_thermo_s;


typedef struct
{
  double data[MAX_EGT_SENSORS];
}rmu_ctrl_sensors_pressure_s;


typedef struct
{

  rmu_ctrl_sensors_pressure_s       egp;
  rmu_ctrl_sensors_thermo_s         egt;
  
}rmu_ctrl_sensors_s;

rmu_ctrl_sensors_s rmu_ctrl_sensors;

class DataLogger {
  public: //accessible from anywhere

    DataLogger(int nKType, int nPrSen, int nSHT20, int nSHT10, int nSEN0220, int stageNum, bool preheat);
    String getRTCValue();
    bool initialize(); // initalize the RTC, sensors, etc.
    void sweep(); //update with live sensor values
    void assess(); //makes recommendations and changes based on sensor data
    double getThermoVal(int subIndex); //the subindex is the # out of the # of that type of component
    double getPrVal(int subIndex, int maxVal);
    double getSHT20Humidity(int subIndex);
    double getSHT20Temperature(int subIndex);
    double getSHT20DewPoint(int subIndex);
    double getSHT10Humidity(int subIndex);
    double getSHT10Temperature(int subIndex);
    double getSHT10DewPoint(int subIndex);
    double getSEN0220Val(int subIndex);
    double getSEN0220Error(double value);
    String getCycleDataAsString();
    void setFan(bool effect);
    void setHeater(bool effect);

  private: //only accessible from inside the class (e.g. in DataLogger::getPressureVal function)

    String makeTwoDigits(String original);
    int indexToSensorType(int index);
    int indexToSensorSubIndex(int index);
    double getSensorVal(int index);
    int psumIndexTracker(int index);
    void assessSensor(int index);
    bool initRTC();
    bool initSensors();
    bool initSEN0220();
    RTC_PCF8523 rtc;
    uFire_SHT20 sht20;
    int thermocouples[MAX_EGT_SENSORS] = {A0, 
                                          A1, 
                                          A2, 
                                          A3,
                                          A4, 
                                          A5, 
                                          A6, 
                                          A7};
    /* Humidity Sensor Info */
    SHT1x sht10s[3] = {SHT1x(SH1X_DO_0,SH1X_CLK_0), 
                       SHT1x(SH1X_DO_1,SH1X_CLK_1), 
                       SHT1x(SH1X_DO_2,SH1X_CLK_2)};
    
    int matchAnalogPinToID[7] = {A0, A1, A2, A3, A4, A5, A6};
    int indexTracker[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //compiled list of number of each type of sensor [numKType,numPr30Sen,numPr2kSen,numSHT20,numSEN0220,...]
    int numSensors; //double counts the SHT20
    int numKType; //number of K type thermocouple
    int numPr30Sen; //number of 30PSI analog pressure sensors
    int numPr2kSen; //number of 2KPSI analog pressure sensors
    int numSHT20; //number of temperature/humidity sensors
    int numSHT10;
    int numSEN0220; //number of 5% CO2 sensors
    const int preheatTime = 180; //number of seconds for the CO2 sensor to preheat
    const unsigned char hexdata[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; //Read the gas density command /Don't change the order
    double cycleData[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //array with sensor data
    const int heaterPin = 45;
    const int fanPin = 46;
    int stage;
    bool preheat;
};



//create DataLogger object
DataLogger dataLog(MAX_EGT_SENSORS, MAX_PR0_30_SENSORS, MAX_SHT20_SENSORS, MAX_SHT10_SENSORS, 1, 0, false); //(thermocouples, 30psi pressure, SHT20, SHT10, SEN0220, stage #, should it preheat for 3 minutes?)

//filename for the SD card
String file;

//time in ms between each round of sensor readings
//will run into issues if kept on for more than 40 days
const int T = 2000;

//time since start in ms
long localTime = 0;

//stores time of most recent sensor sweep in ms; won't be accurate for the first sweep
long lastSensorCheck = 0;

//stage in sorption cycle
//0 = adsorption
//1 = preheating
//2 = desorption
//3 = precooling
byte stage = 0;


//////////////////////////////////////////////////////////////////////////////////////
//PINS
//////////////////////////////////////////////////////////////////////////////////////

//SD card reader/writer
//MOSI/DI - pin 51
//MISO/DO - pin 50
//CLK/SCK - pin 52
const int chipSelect = 53;
//CS is only for detecting SD card; do NOT use

//Real Time Clock
//SDA
//SCL

//SHT20
//SDA
//SCL

//SHT10
//PWM pins

//SEN0220 uses Serial2
//D16
//D17
//RX connects to RX, TX connects to TX

//pin 45 for heater and pin 46 for the fan;
//const int heater = 45;
//const int fan = 46;

//see "VARIABLES" section for thermocouple pins (pins 2 through 48 except 20 and 21)

//see "VARIABLES" section for pressure sensor pins (pins A0 through A6)

//Using Serial (Digital IO 0 and 1) and GND for RF

//////////////////////////////////////////////////////////////////////////////////////
//SETUP
//////////////////////////////////////////////////////////////////////////////////////

void setup() {
  //might have to jump from 9600 to 19200 for the RF to work
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);

  Sprint("Welcome to a_DATA_LOGGER_TO_RF");

  Sprint("Serials initialized");

  while (CAN_OK != CAN.begin(CAN_BAUD_RATE_0)) 
  {    
    // init can bus : baudrate = 500k
    Sprint("CAN init fail, retry...");
    delay(100);
  }
  Sprint("CAN init success.. baud_rate: ");
  Sprintln(CAN_BAUD_RATE_0);
  
  //TODO create RF_INIT 
  // the RF sensor shouldn't need to be initialized

  //init SDcard
 initSD();

  //if major item can't be initialized, then error out
  if (!dataLog.initialize()) {
    Sprint("Init Error");
    errorState();
  }

  Sprint("done with startup");

  //starts loop of sensor data gathering
}

//////////////////////////////////////////////////////////////////////////////////////
//LOOP
//////////////////////////////////////////////////////////////////////////////////////

void loop() {

  //looks at every sensor value
  Sprint("beginning sweep");
  dataLog.sweep();

  //sends data to SD card and prints to Serial
  Sprint("storing data");
  storeData();

  //could do stuff here if interested in a particular sensor value result
  Sprint("assessing data");
  dataLog.assess();

  delay(SENSOR_SWEEP_INTERVAL_IN_MS); //helps with stability

  /* Fetch CAN BUS Data */
  // variables used for CAN messages
  unsigned char len = 0;
  unsigned char buf[8];
   while(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
      CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

      unsigned long canId = CAN.getCanId();            // get CAN id to match to task

      switch(canId)
      {
        // Control for Throttle Servo //////////
        case EGT_CAN_ID_0_1:  
        case EGT_CAN_ID_2_3: 
        case EGT_CAN_ID_4_5: 
        case EGT_CAN_ID_6_7: 
        case EGT_CAN_ID_8_9: 
        case EGT_CAN_ID_10_11:
        case EGT_CAN_ID_12_13:
        case EGT_CAN_ID_14_15:
        {
          break;
        }
      }

    }

}

//////////////////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////
void initSD() {
  int initTime = 5;
#if 0
  for (int i = 0; i < initTime; i++) {
    Serial.print(initTime - i);
    Sprint(" second(s) remain for SD reader initialization");
    delay(1000);
  }
  
  if (!SD.begin(chipSelect)) { //consider setting pin 53 as output HIGH and/or creating a 0 bit file on the card
    Sprint("Card failed, or not present");
    // don't do anything more:
    errorState();
  }
  #endif 
}

//stores data after every sensor sweep
void storeData() {
  String data = dataLog.getCycleDataAsString();
  Sprint("data = ");
  Sprintln(data);
  Sprint("file = ");
  Sprintln(file);
  Serial1.println(data);
#if 0
  while(file == "") {
    file = createFileName();
  }

  delay(10);

  File dataFile = SD.open(file, FILE_WRITE);

  delay(10);

  if (!dataFile) {
    Sprint("File opening failed.");
  }

  //print cycleOutput to serial (RF) and SD card
  //for this setup, serial messages go through the radio to the computer
  Serial1.println(data);
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
  }
  else {
    Sprint("Data storage failed.");
  }
#endif 
}

String createFileName() {
  String full = dataLog.getRTCValue();
  String partial = "";
  //`YYYYoMMdDDhHHmMMsSS`
  partial += full[5];
  partial += full[6];
  partial += full[8];
  partial += full[9];
  partial += full[11];
  partial += full[12];
  partial += full[14];
  partial += full[15];
  partial += ".txt";
  Sprint("File name is ");
  Sprint(partial);

  return partial;
}

/*! Input adc_raw_value and the Min & Max of the Range of the particular unit of interest. 
    Returns the value within range for the respective adc_val 
    Required: min_range < max_range */
double adc_range_based_interpreter(int adc_val, double min_range, double max_range) {

  double total_range = max_range - min_range;
  ASSERT(max_range > min_range);
  return ((((double)adc_val/ADC_MAX_RESOULTION) * (total_range)) + min_range);

}

void errorState() {
  /* Todo Flash an LED */
  while (1) {
    delay(1000);
  }
}

//////////////////////////////////////////////////////////////////////////////////////
//INTERRUPT FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////
