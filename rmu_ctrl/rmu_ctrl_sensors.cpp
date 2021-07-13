/* RMU Libraries */
#include "rmu_utils.h"
#include "rmu_ctrl_sensors.h"
#include "rmu_ctrl_defs.h"


rmu_ctrl_sensors_s rmu_ctrl_sensors;




void rmu_ctrl_sensors_parse_egt_data(uint32_t can_id, uint32_t len, uint8_t * buf)
{

  
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

  Sprint("CAN Thermo_data_0: ");
  Sprintln(thermo_data_0);
  Sprint("CAN Thermo_data_1: ");
  Sprintln(thermo_data_1);
  
}




