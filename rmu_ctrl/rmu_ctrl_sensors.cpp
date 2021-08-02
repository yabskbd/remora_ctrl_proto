/* RMU Libraries */
#include "rmu_utils.h"
#include "rmu_ctrl_sensors.h"
#include "rmu_ctrl_defs.h"


rmu_ctrl_sensors_s   rmu_ctrl_sensors;
rmu_ctrl_sensors_s * rmu_ctrl_sensors_ptr = &rmu_ctrl_sensors;

// Specify data and clock connections and instantiate SHT1x object
#define clockPin 6 //yellow
#define dataPin  7 //black

SHT1x sht1x(dataPin, clockPin);


void rmu_ctrl_sesnors_init()
{
  memset(rmu_ctrl_sensors_ptr, 0x0, sizeof(rmu_ctrl_sensors_s));
}


int rmu_ctrl_sensors_sweep()
{
  /* Peform Sweep only once we have all temp data */
  rmu_ctrl_sensors_thermo_s * egt_info_ptr = &(rmu_ctrl_sensors_ptr->egt_info);
  bool is_sweep_done = FALSE;
  if (egt_info_ptr->recv_egt_bmsk == RMU_CTRL_SENSORS_EGT_ALL_RECV_BMSK)
  {
    //looks at every sensor value
    Sprint_HP("beginning sweep 0x");
    Sprintln_ext_HP(egt_info_ptr->recv_egt_bmsk, HEX);
    /* Preassure Sensor Sweep */
    rmu_ctrl_sensors_egp_fetch_data();

    /* Humidity Sensor Sweep */
    //rmu_ctrl_sensors_egh_fetch_data();

    /* Fan Data Sweep */
    rmu_ctrl_sensors_ptr->fan_data[0] = digitalRead(RMU_CTRL_DEFS_FAN_DI_0);

    /* Reset recv_bits */
    egt_info_ptr->recv_egt_bmsk = 0;

    /* Data Sweep compelete */
    is_sweep_done = TRUE;
  }
  return is_sweep_done;
}

void rmu_ctrl_sensors_parse_egt_data(uint32_t can_id, uint32_t len, uint8_t * buf)
{

  rmu_ctrl_sensors_thermo_s * egt_info_ptr = &(rmu_ctrl_sensors_ptr->egt_info);

  /* Since CAN_ID are defined by use we will define the first two bytes as the index */
  uint32_t egt_sesnor_id_0 = (can_id & 0xFF) << 1;
  uint32_t egt_sensor_id_1 = egt_sesnor_id_0 + 1;
  
  uint32_t thermo_data_0 = 0;
  uint32_t thermo_data_1 = 0;

  ASSERT(len == 8);
  thermo_data_0 = ( ((uint32_t)(buf[1]) << 16 )| 
                    ((uint32_t)(buf[2]) << 8 )  | 
                    ((uint32_t)(buf[3])) );
  
  thermo_data_1 =  ( ((uint32_t)(buf[5]) << 16 )| 
                     ((uint32_t)(buf[6]) << 8 )  | 
                     ((uint32_t)(buf[7])) );


  egt_info_ptr->data[egt_sesnor_id_0] = thermo_data_0 / RMU_CTRL_SENSORS_EGT_CAN_DATA_SCALING;
  egt_info_ptr->data[egt_sensor_id_1] = thermo_data_1 / RMU_CTRL_SENSORS_EGT_CAN_DATA_SCALING;
  egt_info_ptr->recv_egt_bmsk |= (1 << egt_sesnor_id_0) | ( 1 << egt_sensor_id_1);
  Sprint("CAN Thermo_idx: ");
  Sprintln(egt_sesnor_id_0);
  Sprint("CAN Thermo_data_0: ");
  Sprintln(thermo_data_0);
  Sprint("CAN Thermo_data_1: ");
  Sprintln(thermo_data_1);
  Sprint("CAN recv_egt_bmsk: ");
  Sprintln_ext(egt_info_ptr->recv_egt_bmsk, HEX);
  
}


void rmu_ctrl_sensors_store_egt_data(uint32_t can_id, uint32_t len, uint8_t * buf)
{

  rmu_ctrl_sensors_thermo_s * egt_info_ptr = &(rmu_ctrl_sensors_ptr->egt_info);
  
}

void rmu_ctrl_sensors_egh_fetch_data()
{

  rmu_ctrl_sensors_humidity_s * egh_info_ptr = &(rmu_ctrl_sensors_ptr->egh_info);
  double hum_val, thermo_val;


  float temp_c;
  float temp_f;
  float humidity;

  // Read values from the sensor
  temp_c = sht1x.readTemperatureC();
  humidity = sht1x.readHumidity();

  // Print the values to the serial port
  Serial.print("Temperature: ");
  Serial.print(temp_c, DEC);
  Serial.print("C / ");

  Serial.print(humidity);
  Serial.println("%");

  egh_info_ptr->data[0].humidity    = humidity;
  egh_info_ptr->data[0].thermo      = temp_c;
    

}


void rmu_ctrl_sensors_egp_fetch_data()
{
  rmu_ctrl_sensors_pressure_s * egp_info_ptr = &(rmu_ctrl_sensors_ptr->egp_info);
  int adc_pin, adc_val;
  double gague_psi;
  
  for (uint32_t ept_tbl_idx = 0; ept_tbl_idx < MAX_EGP_SENSORS; ++ept_tbl_idx)
  {
    adc_pin = egp_info_ptr->adc_pins_tbl[ept_tbl_idx];
    adc_val = analogRead(adc_pin);

    if(ept_tbl_idx < MAX_EGP_NEG15_15_SENSORS)
    {
      gague_psi = rmu_utils_adc_range_based_interpreter(adc_val, -15, 15, RMU_CTRL_DEFS_EGP_ADC_VAL_OFFSET);
    }
    else if(ept_tbl_idx < MAX_EPG_30_SENSORS)
    {
      gague_psi = rmu_utils_adc_range_based_interpreter(adc_val, 0, 30, RMU_CTRL_DEFS_EGP_ADC_VAL_OFFSET);
    }
    else if(ept_tbl_idx < MAX_EGP_2K_SENSORS)
    {
      gague_psi = rmu_utils_adc_range_based_interpreter(adc_val, 0, 2000, RMU_CTRL_DEFS_EGP_ADC_VAL_OFFSET);
    }

    egp_info_ptr->data[ept_tbl_idx] = gague_psi;
  }

}




rmu_ctrl_sensors_s * rmu_ctrl_sensors_get_ptr()
{
  return rmu_ctrl_sensors_ptr;
}



