#ifndef RMU_CTRL_SENSORS
#define RMU_CTRL_SENSORS
//////////////////////////////////////////////////////////////////////////////////////
//Variables
//////////////////////////////////////////////////////////////////////////////////////

#include "SHT1x.h"
#include "rmu_ctrl_defs.h"


#define RMU_CTRL_SENSORS_EGT_CAN_DATA_SCALING        100.0

#define RMU_CTRL_SENSORS_EGT_ALL_RECV_BMSK           ((1 << MAX_EGT_SENSORS) - 1)


#define RMU_CTRL_SENSORS_EGH_SLOPE  0.03892
#define RMU_CTRL_SENSORS_EGH_OFFSET -42.017


enum {

AT_EXHAUST = 0,
EVAPO,




} rmu_ctrl_senosrs_egt_location;

typedef struct
{
  double data[MAX_EGT_SENSORS];
  uint32_t recv_egt_bmsk;
  
}rmu_ctrl_sensors_thermo_s;


typedef struct
{
  double data[MAX_EGP_SENSORS];
  /*Arrange in NEG15_15 then 30, then 2K */
  uint8_t adc_pins_tbl[MAX_EGP_SENSORS] = {A0, 
                                      A1, 
                                      A2, 
                                      A3,
                                      A4, 
                                      A5, 
                                      A7, 
                                      A8,
                                      A9}; 
}rmu_ctrl_sensors_pressure_s;


typedef struct
{

  float thermo;
  float humidity;
                                      
}rmu_ctrl_sensors_humidity_data_s;


typedef struct
{

  rmu_ctrl_sensors_humidity_data_s data[RMU_CTRL_DEFS_MAX_EGH_SENSORS];
  uint8_t adc_pins_tbl[RMU_CTRL_DEFS_MAX_EGH_SENSORS] = {A10, A11};

                                      
}rmu_ctrl_sensors_humidity_s;



typedef struct
{

  rmu_ctrl_sensors_pressure_s            egp_info;
  rmu_ctrl_sensors_thermo_s              egt_info;
  rmu_ctrl_sensors_humidity_s            egh_info;
  /* Record Fan on or off */
  uint32_t fan_data[RMU_CTRL_DEFS_MAX_FAN];
  
}rmu_ctrl_sensors_s;


void rmu_ctrl_sesnors_init();


int rmu_ctrl_sensors_sweep();

void rmu_ctrl_sensors_egh_fetch_data();


void rmu_ctrl_sensors_egp_fetch_data();


void rmu_ctrl_sensors_parse_egt_data(uint32_t can_id, uint32_t len, uint8_t * buf);

void rmu_ctrl_sensors_store_egt_data(uint32_t can_id, uint32_t len, uint8_t * buf);



rmu_ctrl_sensors_s * rmu_ctrl_sensors_get_ptr();




#endif
