#ifndef RMU_CTRL_DEFS
#define RMU_CTRL_DEFS

/*! ADC Max Resolution in bits
    For Arduino ref: https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/ 
    Mega Resolution is 10bits i.e 0 - 1023 */
#define RMU_CTRL_DEFS_MAX_VOLTAGE      5
#define ADC_MAX_RESOLUTION_IN_BITS     10
#define ADC_MAX_RESOULTION             ((1 << ADC_MAX_RESOLUTION_IN_BITS) - 1)

/* --------------------------------------------------CAN BUS----------------------------------------------------------*/

/*! CAN Bus Definitions */
// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
#define SPI_CS_PIN            9
#define CAN_BAUD_RATE         CAN_250KBPS

/* EGT CAN ID Def. */ 
#define EGT_CAN_ID_0_1             0x100
#define EGT_CAN_ID_2_3             0x101
#define EGT_CAN_ID_4_5             0x102
#define EGT_CAN_ID_6_7             0x103

#define EGT_CAN_ID_8_9             0x104
#define EGT_CAN_ID_10_11           0x105
#define EGT_CAN_ID_12_13           0x106
#define EGT_CAN_ID_14_15           0x107


#define EGT_CAN_ID_0             EGT_CAN_ID_0_1
#define EGT_CAN_ID_1             EGT_CAN_ID_0_1
#define EGT_CAN_ID_2             EGT_CAN_ID_2_3
#define EGT_CAN_ID_3             EGT_CAN_ID_2_3
#define EGT_CAN_ID_4             EGT_CAN_ID_4_5
#define EGT_CAN_ID_5             EGT_CAN_ID_4_5
#define EGT_CAN_ID_6             EGT_CAN_ID_6_7
#define EGT_CAN_ID_7             EGT_CAN_ID_6_7
#define EGT_CAN_ID_8             EGT_CAN_ID_8_9
#define EGT_CAN_ID_9             EGT_CAN_ID_8_9
#define EGT_CAN_ID_10            EGT_CAN_ID_10_11
#define EGT_CAN_ID_11            EGT_CAN_ID_10_11
#define EGT_CAN_ID_12            EGT_CAN_ID_12_13
#define EGT_CAN_ID_13            EGT_CAN_ID_12_13
#define EGT_CAN_ID_14            EGT_CAN_ID_14_15
#define EGT_CAN_ID_15            EGT_CAN_ID_14_15


/* --------------------------------------------------Sensor Max Value Definitions----------------------------------------------------------*/

/*! Max Definitions */
/*! MAX Sensor Counts */

/*! Exhaust Gas Temprature Sensors */
#define MAX_EGT_SENSORS                8

/*! Exhaust Gas Preassure Sensors */
#define MAX_EGP_NEG15_15_SENSORS                0
#define MAX_EPG_30_SENSORS                      9
#define MAX_EGP_2K_SENSORS                      0
#define MAX_EGP_SENSORS                         (MAX_EGP_NEG15_15_SENSORS + MAX_EPG_30_SENSORS + MAX_EGP_2K_SENSORS)

#define RMU_CTRL_DEFS_MAX_EGH_SENSORS           1


#define RMU_CTRL_DEFS_EGP_OFFSET_VOLTAGE          0.5
#define RMU_CTRL_DEFS_EGP_ADC_VAL_OFFSET          (double)((RMU_CTRL_DEFS_EGP_OFFSET_VOLTAGE / RMU_CTRL_DEFS_MAX_VOLTAGE ) * ADC_MAX_RESOULTION)

#define RMU_CTRL_DEFS_MAX_FAN                   1
/* -------------------------------------------------------------------------------------------------------------------*/
/*! Pin Mappings */
/* Thermocouple Serial Pins for CLK, CS, DO */
#define EGT_CLK_0       8
#define EGT_DO_0        9

/* Update to use new GPIO Pins */
#define EGT_CLK_1       8
#define EGT_DO_1        9


#define EGT_CS_0             10
#define EGT_CS_1             11
#define EGT_CS_2             12
#define EGT_CS_3             13
#define EGT_CS_4             22
#define EGT_CS_5             23
#define EGT_CS_6             24
#define EGT_CS_7             25
#define EGT_CS_8             9


/* Humidity Sensor SH1x CLK and DO Pin (PWM) */
#define SH1X_CLK_0            6
#define SH1X_DO_0             7

#define SH1X_CLK_1            4
#define SH1X_DO_1             5

#define SH1X_CLK_2            6
#define SH1X_DO_2             7

/*! Fan Digital PIN In */
#define RMU_CTRL_DEFS_FAN_DI_0   32
/* -------------------------------------------------------------------------------------------------------------------*/

/*! Temprature Range Scaling */
#define MAX_TEMP_C_DEGREE      500
#define MIN_TEMP_C_DEGREE      0

/*! Max Sensor Definitions */
#define RMU_CTRL_DEF_ONE_SEC_IN_MICRO_SEC      1000000

#define RMU_CTRL_DEF_SENSOR_SWEEP_INTERVAL     2*RMU_CTRL_DEF_ONE_SEC_IN_MICRO_SEC



#define MAX_NUM_TEMP 11
#define MAX_NUM_PSI   5
#define DATA_LOG_BUF_SIZE (MAX_TEMP + MAX_PSI)

#endif
