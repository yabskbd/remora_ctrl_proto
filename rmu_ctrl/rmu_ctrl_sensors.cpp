/* RMU Libraries */
#include "rmu_utils.h"
#include "rmu_ctrl_sensors.h"
#include "rmu_ctrl_defs.h"


rmu_ctrl_sensors_s   rmu_ctrl_sensors;
rmu_ctrl_sensors_s * rmu_ctrl_sensors_ptr = &rmu_ctrl_sensors;




void rmu_ctrl_sensors_parse_egt_data(uint32_t can_id, uint32_t len, uint8_t * buf)
{

  rmu_ctrl_sensors_egt_s * egt_info_ptr = &(rmu_ctrl_sensors_ptr->egt_info);

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



