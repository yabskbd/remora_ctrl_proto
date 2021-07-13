#ifndef RMU_CTRL_SENSORS
#define RMU_CTRL_SENSORS
//////////////////////////////////////////////////////////////////////////////////////
//Variables
//////////////////////////////////////////////////////////////////////////////////////

#include "rmu_ctrl_defs.h"

#define RMU_CTRL_SENSORS_EGT_CAN_DATA_SCALING        100.0

typedef struct
{
  double data[MAX_EGT_SENSORS];
}rmu_ctrl_sensors_egt_s;


typedef struct
{
  double data[MAX_EGT_SENSORS];
}rmu_ctrl_sensors_pressure_s;


typedef struct
{

  rmu_ctrl_sensors_pressure_s       egp_info;
  rmu_ctrl_sensors_egt_s            egt_info;
}rmu_ctrl_sensors_s;



void rmu_ctrl_sensors_parse_egt_data(uint32_t can_id, uint32_t len, uint8_t * buf);


rmu_ctrl_sensors_s * rmu_ctrl_sensors_get_ptr();




#endif
