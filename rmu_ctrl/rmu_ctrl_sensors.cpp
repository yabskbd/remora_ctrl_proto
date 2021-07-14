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


void rmu_ctrl_sensors_sweep()
{

  rmu_ctrl_sensors_egp_fetch_data();
  rmu_ctrl_sensors_egh_fetch_data();
 // rmu_ctrl_sensors_ptr->fan_data[0] = digitalRead(RMU_CTRL_DEFS_FAN_DI_0);
  

}

void rmu_ctrl_sensors_egh_fetch_data()
{
#if 0
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
    
#endif
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
      gague_psi = adc_range_based_interpreter(adc_val, -15, 15);
    }
    else if(ept_tbl_idx < MAX_EPG_30_SENSORS)
    {
      gague_psi = adc_range_based_interpreter(adc_val, 0, 30);
    }
    else if(ept_tbl_idx < MAX_EGP_2K_SENSORS)
    {
      gague_psi = adc_range_based_interpreter(adc_val, 0, 2000);
    }

    egp_info_ptr->data[ept_tbl_idx] = gague_psi;
  }

}


void rmu_ctrl_sensors_parse_egt_data(uint32_t can_id, uint32_t len, uint8_t * buf)
{

  rmu_ctrl_sensors_thermo_s * egt_info_ptr = &(rmu_ctrl_sensors_ptr->egt_info);

  /* Since CAN_ID are defined by use we will define the first two bytes as the index */
  uint32_t egt_sesnor_id_0 = (can_id & 0xFF) * 2;
  uint32_t egt_sensor_id_1 = egt_sesnor_id_0 + 1;
  
  uint32_t thermo_data_0 = 0;
  uint32_t thermo_data_1 = 0;

  ASSERT(len == 8);
  uint32_t bytes_in_word = 4;
  uint32_t bits_in_byte  = 8;

  
  for (uint32_t buf_idx = 0; buf_idx < bytes_in_word; ++buf_idx)
  {
    thermo_data_0 |= buf[buf_idx] << ((bytes_in_word - buf_idx - 1) * bits_in_byte);
  
  }

  for (uint32_t buf_idx = 0; buf_idx < bytes_in_word; ++buf_idx)
  {
    thermo_data_1 |= buf[buf_idx + bytes_in_word] << ((bytes_in_word - buf_idx - 1) * bits_in_byte);
  }
  egt_info_ptr->data[egt_sesnor_id_0] = thermo_data_0 / RMU_CTRL_SENSORS_EGT_CAN_DATA_SCALING;
  egt_info_ptr->data[egt_sensor_id_1] = thermo_data_1 / RMU_CTRL_SENSORS_EGT_CAN_DATA_SCALING;
  
  Sprint("CAN Thermo_idx: ");
  Sprintln(egt_sesnor_id_0);
  Sprint("CAN Thermo_data_0: ");
  Sprintln(thermo_data_0);
  Sprint("CAN Thermo_data_1: ");
  Sprintln(thermo_data_1);
  
}



rmu_ctrl_sensors_s * rmu_ctrl_sensors_get_ptr()
{
  return rmu_ctrl_sensors_ptr;
}



