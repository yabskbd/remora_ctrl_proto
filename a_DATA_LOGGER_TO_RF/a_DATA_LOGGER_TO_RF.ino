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
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include "Adafruit_MAX31855.h"
#include "uFire_SHT20.h"
#include "SHT1x.h"

/*! MAX Sensor Counts */
#define MAX_EGT_SENSORS                8
#define MAX_PR0_30_SENSORS             5
#define MAX_PR0_2K_SENSORS             0

#define MAX_SHT10_SENSORS              2
#define MAX_SHT20_SENSORS              0




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


/*! ADC Max Resolution in bits
    For Arduino ref: https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/ 
    Mega Resolution is 10bits i.e 0 - 1023 */
#define ADC_MAX_RESOLUTION_IN_BITS     10

/*! Temprature Range Scaling */
#define MAX_TEMP_C_DEGREE      500
#define MIN_TEMP_C_DEGREE      0

/*! Max Sensor Defination */
#define MAX_NUM_TEMP 11
#define MAX_NUM_PSI   5
#define DATA_LOG_BUF_SIZE (MAX_TEMP + MAX_PSI)

//////////////////////////////////////////////////////////////////////////////////////
//Variables
//////////////////////////////////////////////////////////////////////////////////////

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
    Adafruit_MAX31855 thermocouples[MAX_EGT_SENSORS] = {Adafruit_MAX31855(EGT_CLK_0, EGT_CS_0, EGT_DO_0), 
                                          Adafruit_MAX31855(EGT_CLK_0, EGT_CS_1, EGT_DO_0), 
                                          Adafruit_MAX31855(EGT_CLK_0, EGT_CS_2, EGT_DO_0), 
                                          Adafruit_MAX31855(EGT_CLK_0, EGT_CS_3, EGT_DO_0),
                                          Adafruit_MAX31855(EGT_CLK_0, EGT_CS_4, EGT_DO_0), 
                                          Adafruit_MAX31855(EGT_CLK_0, EGT_CS_5, EGT_DO_0), 
                                          Adafruit_MAX31855(EGT_CLK_0, EGT_CS_6, EGT_DO_0), 
                                          Adafruit_MAX31855(EGT_CLK_0, EGT_CS_7, EGT_DO_0)};
    /* Humidity Sensor Info */
    SHT1x sht10s[3] = {SHT1x(SH1X_CLK_0,SH1X_DO_0), 
                       SHT1x(SH1X_CLK_1,SH1X_DO_1), 
                       SHT1x(SH1X_CLK_2,SH1X_DO_2)};
    
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
  while (!Serial) {
    delay(1);
  }

  //might have to jump from 9600 to 19200 for the RF to work
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);

  Serial.println("Welcome to a_DATA_LOGGER_TO_RF");

  Serial.println("Serials initialized");

  //dataLog = new DataLogger(16,5,1,1,1,0,false);

  Serial.println("Data object created");

  //the RF sensor shouldn't need to be initialized

  //init SDcard
  initSD();

  //if major item can't be initialized, then error out
  if (!dataLog.initialize()) {
    Serial.println("Init Error");
    errorState();
  }

  Serial.println("done with startup");

  //starts loop of sensor data gathering
}

//////////////////////////////////////////////////////////////////////////////////////
//LOOP
//////////////////////////////////////////////////////////////////////////////////////

void loop() {
  //find and set the local time
  localTime = millis();

  //determines period of sensor data
  //at some point, the response time of the sensors means that going faster doesn't matter
  if (localTime - lastSensorCheck >= T) {
    lastSensorCheck = localTime;

    //looks at every sensor value
    Serial.println("beginning sweep");
    dataLog.sweep();

    //sends data to SD card and prints to Serial
    Serial.println("storing data");
    storeData();

    //could do stuff here if interested in a particular sensor value result
    Serial.println("assessing data");
    dataLog.assess();

    delay(1); //helps with stability
  }

  Serial.println("new loop");

}

//////////////////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////
void initSD() {
  int initTime = 5;
#if 0
  for (int i = 0; i < initTime; i++) {
    Serial.print(initTime - i);
    Serial.println(" second(s) remain for SD reader initialization");
    delay(1000);
  }
  
  if (!SD.begin(chipSelect)) { //consider setting pin 53 as output HIGH and/or creating a 0 bit file on the card
    Serial.println("Card failed, or not present");
    // don't do anything more:
    errorState();
  }
  #endif 
}

//stores data after every sensor sweep
void storeData() {
  String data = dataLog.getCycleDataAsString();
  Serial.print("data = ");
  Serial.println(data);
  Serial.print("file = ");
  Serial.println(file);
  Serial1.println(data);
#if 0
  while(file == "") {
    file = createFileName();
  }

  delay(10);

  File dataFile = SD.open(file, FILE_WRITE);

  delay(10);

  if (!dataFile) {
    Serial.println("File opening failed.");
  }

  //print cycleOutput to serial (RF) and SD card
  //for this setup, serial messages go through the radio to the computer
  Serial1.println(data);
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
  }
  else {
    Serial.println("Data storage failed.");
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
  Serial.print("File name is ");
  Serial.println(partial);

  return partial;
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
