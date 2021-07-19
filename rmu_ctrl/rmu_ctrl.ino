
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
#define RMU_CTRL_SWEEP_INTERNVAL_IN_MS_0 6000

#define RMU_CTRL_SWEEP_INTERNVAL_IN_MS_1 (RMU_CTRL_SWEEP_INTERNVAL_IN_MS_0/2)


static uint32_t curr_sweep_internval = 0;


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

static bool is_first_run = TRUE;

static uint8_t  can_data_buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};


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
  
  rmu_ctrl_sesnors_init();

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
  pinMode(RMU_CTRL_DEFS_FAN_DI_0, INPUT);    // sets the digital FAN PIN as input
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
  schedule_sweep();

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

    CAN.readMsgBuf(&len, can_data_buf);    // read data,  len: data length, buf: data buf
    
    unsigned long can_id = CAN.getCanId();            // get CAN id to match to task
    
    Sprint("-----------------------------");
    Sprint_HP("Get data from ID: 0x");
    Sprintln_ext_HP(can_id, HEX);
    
#ifdef RMU_UTILS_EN_LP_PRINT
    for (int i = 0; i < len; i++) { // print the data
        Sprint_ext(can_data_buf[i], HEX);
        Sprint("\t");
    }
    Sprintln();
#endif

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



  int ret_val_start_store = rmu_ctrl_sensors_sweep();
  
  //sends data to SD card and prints to Serial
  if (ret_val_start_store == TRUE)
  {
    Sprintln("storing data");
      storeData();

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
  for(uint32_t egt_cnt = 0; egt_cnt < MAX_EGT_SENSORS; egt_cnt++) {
    hdr  += "EGT_C_" + String(egt_cnt) + ",";
    data += ctrl_sensors_ptr->egt_info.data[egt_cnt];
    data += ",";
  }

  /* Collect EGP Sensors */
  for(uint32_t egp_cnt = 0; egp_cnt < MAX_EGP_SENSORS; egp_cnt++)
  {
    hdr  += "EGP_PSI_" + String(egp_cnt) + ",";
    data += ctrl_sensors_ptr->egp_info.data[egp_cnt];
    data += ",";
  }

  for(uint32_t egh_cnt = 0; egh_cnt < RMU_CTRL_DEFS_MAX_EGH_SENSORS; egh_cnt++)
  {
    hdr  += "EGH_in_HUM_" + String(egh_cnt) + ",";
    data += ctrl_sensors_ptr->egh_info.data[egh_cnt].humidity;
    data += ",";

    hdr  += "EGH_in_Thermo_C_" + String(egh_cnt) + ",";
    data += ctrl_sensors_ptr->egh_info.data[egh_cnt].thermo;
    data += ",";
  }
   hdr += "Fan_0";
   data += ctrl_sensors_ptr->fan_data[0];

  /* Send to RF */
  if(is_first_run)
  {
    Sprintln_Output_Debug(hdr);
    Serial1.println(hdr);
    is_first_run = FALSE;
  }
  
  Sprintln_Output_Debug(data);
  Serial1.println(data);
}



void errorState() {
  /* Todo Flash an LED */
  while (1) {
    delay(1000);
  }
}
