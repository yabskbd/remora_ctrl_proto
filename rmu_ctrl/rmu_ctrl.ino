
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
const int T = 3000;

//time since start in ms
long localTime = 0;

//stores time of most recent sensor sweep in ms; won't be accurate for the first sweep
long last_sensor_sweep_ms = 0;

//stage in sorption cycle
//0 = adsorption
//1 = preheating
//2 = desorption
//3 = precooling
byte stage = 0;


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

  last_sensor_sweep_ms = millis();

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

  Sprintln("done with startup");

  //starts loop of sensor data gathering
}

//////////////////////////////////////////////////////////////////////////////////////
//LOOP
//////////////////////////////////////////////////////////////////////////////////////

void loop() {

  /* Fetch CAN BUS Data */
  fetch_canbus_data();
  uint32_t curr_ms = millis();
  if(( curr_ms - last_sensor_sweep_ms) > T)
  {
    schedule_sweep();
    last_sensor_sweep_ms = curr_ms;
  }

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


void schedule_sweep()
{

  //looks at every sensor value
  Sprintln("beginning sweep");
  rmu_ctrl_sensors_egp_fetch_data();
  
  //sends data to SD card and prints to Serial
  Sprintln("storing data");
  storeData();

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

  rmu_ctrl_sensors_s * ctrl_sensors_ptr = rmu_ctrl_sensors_get_ptr();

  String data = "";
  String hdr  = "";

  //add date and time to start of string
  // TODO fix RTC
#if 0
  hdr  += "RTC_VALUE,";
  data += getRTCValue();
  data += ",";
#endif 
  //add time in milliseconds to start of string
  hdr  += "MILLIS,";
  data  += String(millis());
  data  += ",";

  /* Collect EGT Sensors */
  for(int egt_cnt = 0; egt_cnt < MAX_EGT_SENSORS; egt_cnt++) {
    hdr  += "EGT_in_C_" + String(egt_cnt) + ",";
    data += ctrl_sensors_ptr->egt_info.data[egt_cnt];
    data += ",";
  }

  /* Collect EGP Sensors */
  for(int egp_cnt = 0; egp_cnt < MAX_EGP_SENSORS; egp_cnt++)
  {
    hdr  += "EPT_in_PSI_" + String(egp_cnt) + ",";
    data += ctrl_sensors_ptr->egp_info.data[egp_cnt];
    data += ",";
  }
  /* Send to RF */
  Sprintln(hdr);
  Sprintln(data);
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

#endif 
}



void errorState() {
  /* Todo Flash an LED */
  while (1) {
    delay(1000);
  }
}
