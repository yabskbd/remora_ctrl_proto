
/* Arduino Libraries */
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

/* External Libraries */
#include "mcp2515_can.h"

/* RMU Libraries */
#include "rmu_utils.h"
#include "rmu_ctrl_defs.h"
#include "rmu_ctrl.h"
#include "rmu_ctrl_log.h"
#include "rmu_ctrl_sensors.h"


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

//create DataLogger object
DataLogger dataLog(0, MAX_PR0_30_SENSORS, MAX_SHT20_SENSORS, MAX_SHT10_SENSORS, 1, 0, false); //(thermocouples, 30psi pressure, SHT20, SHT10, SEN0220, stage #, should it preheat for 3 minutes?)

mcp2515_can CAN(SPI_CS_PIN);


//////////////////////////////////////////////////////////////////////////////////////
//SETUP
//////////////////////////////////////////////////////////////////////////////////////

void setup() {
  //might have to jump from 9600 to 19200 for the RF to work
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);

  Sprintln("Welcome to a_DATA_LOGGER_TO_RF");

  Sprintln("Serials initialized");

  while (CAN_OK != CAN.begin(CAN_BAUD_RATE)) 
  {    
    // init can bus : baudrate = 500k
    Sprintln("CAN init fail, retry...");
    delay(100);
  }
  Sprint("CAN init success.. baud_rate: ");
  Sprintln(CAN_BAUD_RATE);
  
  //TODO create RF_INIT 
  // the RF sensor shouldn't need to be initialized

  //init SDcard
 initSD();

  //if major item can't be initialized, then error out
  if (!dataLog.initialize()) {
    Sprintln("Init Error");
    errorState();
  }

  Sprintln("done with startup");

  //starts loop of sensor data gathering
}

//////////////////////////////////////////////////////////////////////////////////////
//LOOP
//////////////////////////////////////////////////////////////////////////////////////

void loop() {

  /* Fetch CAN BUS Data */
  fetch_canbus_data();

  //looks at every sensor value
  Sprintln("beginning sweep");
  dataLog.sweep();

  //sends data to SD card and prints to Serial
  Sprintln("storing data");
  storeData();

  //could do stuff here if interested in a particular sensor value result
  Sprintln("assessing data");
  dataLog.assess();

  delay(SENSOR_SWEEP_INTERVAL_IN_MS); //helps with stability

}

//////////////////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////

void fetch_canbus_data()
{

  /* Fetch CAN BUS Data */
  // variables used for CAN messages
  uint8_t  len = 0;
  while(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
  {
    uint8_t  can_data_buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    CAN.readMsgBuf(&len, can_data_buf);    // read data,  len: data length, buf: data buf
    
    unsigned long can_id = CAN.getCanId();            // get CAN id to match to task
    
    Sprintln("-----------------------------");
    Sprint("Get data from ID: 0x");
    Sprintln_ext(can_id, HEX);
    
    for (int i = 0; i < len; i++) { // print the data
        Sprint_ext(can_data_buf[i], HEX);
        Sprint("\t");
    }
    Sprintln();
    
    switch(can_id)
    {
      case EGT_CAN_ID_0_1:  
      case EGT_CAN_ID_2_3: 
      case EGT_CAN_ID_4_5: 
      case EGT_CAN_ID_6_7: 
      case EGT_CAN_ID_8_9: 
      case EGT_CAN_ID_10_11:
      case EGT_CAN_ID_12_13:
      case EGT_CAN_ID_14_15:
      {
        rmu_ctrl_sensors_parse_egt_data(can_id, len, &can_data_buf[0]);
        break;
      }
    }

  }

}


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
  Sprintln(partial);

  return partial;
}

void errorState() {
  /* Todo Flash an LED */
  while (1) {
    delay(1000);
  }
}
