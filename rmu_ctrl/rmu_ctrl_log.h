#ifndef RMU_CTRL_LOG
#define RMU_CTRL_LOG

#include <RTClib.h>
#include "rmu_ctrl_defs.h"
#include "rmu_utils.h"

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
    int thermocouples[8] = {A0, 
                                          A1, 
                                          A2, 
                                          A3,
                                          A4, 
                                          A5, 
                                          A6, 
                                          A7};
    
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


#endif
