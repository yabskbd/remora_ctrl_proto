#ifndef RMU_CTRL_SENSORS
#define RMU_CTRL_SENSORS
//////////////////////////////////////////////////////////////////////////////////////
//Variables
//////////////////////////////////////////////////////////////////////////////////////

#include "rmu_ctrl_defs.h"

typedef struct
{
  double data[MAX_EGT_SENSORS];
}rmu_ctrl_sensors_thermo_s;


typedef struct
{
  double data[MAX_EGT_SENSORS];
}rmu_ctrl_sensors_pressure_s;


typedef struct
{

  rmu_ctrl_sensors_pressure_s       egp;
  rmu_ctrl_sensors_thermo_s         egt;
}rmu_ctrl_sensors_s;



void rmu_ctrl_sensors_parse_egt_data(uint32_t can_id, uint32_t len, uint8_t * buf);




#endif
