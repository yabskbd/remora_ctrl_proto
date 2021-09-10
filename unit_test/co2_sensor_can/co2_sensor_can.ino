/****************************************************************************
 * Requeset C02 Sensor for C02 concentration data over serail
 * Convert received data to PPM and sends over CAN BUS with extended CAN 
 * with 250 KBPS Baud Rate. 
 * This is intended to run on Arudino Nano 
 * 
 *****************************************************************************/

#include <SPI.h>
#include <SoftwareSerial.h>
#include "mcp2515_can.h"

/*! SPI pin for CAN Controller Chip */
#define CO2_SENSOR_CAN_SPI_CS_PIN  9 
/*!  Query CO2 Sensor Every 1 seconds. units in milliseconds */
#define CO2_SENSOR_CAN_LOOP_DELAY  1000 
/*! CAN BAUD RATE.
    IMPORTANT: The Hardware for this operates at 250kbps when set to the below MACRO */
#define CO2_SENSOR_CAN_BAUD_RATE CAN_500KBPS
/* J1939 PGN 65320 = 0xFF2800 */
#define CO2_SENSOR_CAN_EXTENDED_ID  0xFF2800
/* RTR Set to zero since sending DATA Bytes */
#define CO2_SENSOR_CAN_RTR     0

/*! MAX CAN Data Bytes. Defined by spec */
#define CO2_SENSOR_CAN_MAX_CAN_DATA_BYTE 8

/*! Software Serial Setup RX_PIN, TX_PIN */
#define CO2_SENSOR_CAN_SW_SERIAL_RX_PIN 2
#define CO2_SENSOR_CAN_SW_SERIAL_TX_PIN 3

/*! undef when in production...define for debuging to enable for Serail Print */
#undef CO2_SENSOR_CAN_ENABLE_SERIAL_PRINT 
/*! Use the below Macros for Serail Print.
   This enabled for high level contorl to disable all Serail Print for production */
#ifdef CO2_SENSOR_CAN_ENABLE_SERIAL_PRINT
#define CO2_SENSOR_CAN_SERIAL_PRINT(OUTPUT_STRING) Serial.print(OUTPUT_STRING)
#define CO2_SENSOR_CAN_SERIAL_PRINTLN(OUTPUT_STRING) Serial.println(OUTPUT_STRING)
#define CO2_SENSOR_CAN_SERIAL_PRINT_EXT(OUTPUT_STRING, format) Serial.print(OUTPUT_STRING, format)
#else
#define CO2_SENSOR_CAN_SERIAL_PRINT(OUTPUT_STRING) 
#define CO2_SENSOR_CAN_SERIAL_PRINTLN(OUTPUT_STRING)
#define CO2_SENSOR_CAN_SERIAL_PRINT_EXT(OUTPUT_STRING, format)
#endif 

/*! Globals */
/*! Software Serial for C02 Sensor Data. RX, TX */
SoftwareSerial co2_sensor_serial(CO2_SENSOR_CAN_SW_SERIAL_RX_PIN, CO2_SENSOR_CAN_SW_SERIAL_TX_PIN); 
/*! Initialze CAN Controller. CAN Set to 250kbps */
mcp2515_can CAN(CO2_SENSOR_CAN_SPI_CS_PIN); 
/*! Conversion multiplier to ppm from CO2 Sensor RX data */
uint32_t co2_sensor_can_ppm_conv = 10;

void setup()
{
    co2_sensor_serial.begin(9600); //Set serial baud rate to 9600 //communicates with the CO2 sensor
    Serial.begin(9600);            //Set serial baud rate to 9600 //provides debugging info through USB port
    {
    while (!Serial)
        CO2_SENSOR_CAN_SERIAL_PRINTLN("Serail init fail, retry...");
        delay(100);
    };

    while (CAN_OK != CAN.begin(CO2_SENSOR_CAN_BAUD_RATE))
    { 
        // init can bus : baudrate = 250kbps (CAN_500KBPS achieves and 250k baudrate)
        CO2_SENSOR_CAN_SERIAL_PRINTLN("CAN init fail, retry...");
        delay(100);
    }
    CO2_SENSOR_CAN_SERIAL_PRINTLN("CAN init ok!");
}

void loop() {

    /* CAN Data TX Buffer. init to all zero for each txs */
    unsigned char can_tx_buf[CO2_SENSOR_CAN_MAX_CAN_DATA_BYTE] = {0, 0, 0, 0, 0, 0, 0, 0}; 
    String co2_sensor_serial_rx;   
    
    /* Request CO2 Sensor for CO2 Concentration Data */
    co2_sensor_serial.write("Z\r\n");
    if (co2_sensor_serial.available() > 0) {
        /* Read avail incoming byte */
        co2_sensor_serial_rx = co2_sensor_serial.readString();
        /* Debug Print for RX data from CO2 Serial */
        CO2_SENSOR_CAN_SERIAL_PRINT("rx: ");
        CO2_SENSOR_CAN_SERIAL_PRINTLN(co2_sensor_serial_rx);
 
        /* Rx Data ex:  'Z 00382'
           grabs the last 5 digits of the returned value
           drops the " Z " or similar prefix
           only keeps the value in decimal form */ 
        String incomingByteShort = co2_sensor_serial_rx.substring(co2_sensor_serial_rx.length() - 5);
        /* Convert RX string to unsinged int */
        uint32_t co2_ppm = (uint32_t)incomingByteShort.toInt();
        /* Convert Serail RX data to PPM */
        /* Debug Print */
        co2_ppm = co2_ppm * co2_sensor_can_ppm_conv;
        CO2_SENSOR_CAN_SERIAL_PRINT("co2_ppm ");
        CO2_SENSOR_CAN_SERIAL_PRINTLN(co2_ppm);
        CO2_SENSOR_CAN_SERIAL_PRINT("size of co2_ppm ");
        CO2_SENSOR_CAN_SERIAL_PRINTLN(sizeof(co2_ppm));
        /* moves data from uint32_t to char array to be TX over CAN */ 
        memcpy(can_tx_buf, &co2_ppm, sizeof(co2_ppm)); 
        /* Send CAN Data to Bus. Data is sent as Little Endian over CAN */
        CO2_SENSOR_CAN_SERIAL_PRINTLN("CAN BUS sendMsgBuf ok!");
        CAN.sendMsgBuf(CO2_SENSOR_CAN_EXTENDED_ID, CO2_SENSOR_CAN_RTR, CO2_SENSOR_CAN_MAX_CAN_DATA_BYTE, can_tx_buf); 

    #ifdef CO2_SENSOR_CAN_ENABLE_SERIAL_PRINT
        /* Debug Priting of CAN MSG buf */
        for (int buf_idx = 0; buf_idx < CO2_SENSOR_CAN_MAX_CAN_DATA_BYTE; buf_idx++)
        { 
            CO2_SENSOR_CAN_SERIAL_PRINT_EXT(can_tx_buf[buf_idx], HEX);
            CO2_SENSOR_CAN_SERIAL_PRINT("\t");
        }
        CO2_SENSOR_CAN_SERIAL_PRINTLN(" ");
    #endif 
            
    }
    /* Loop Dealy, to query CO2 Sensor Data Every X Seconds */
    delay(CO2_SENSOR_CAN_LOOP_DELAY); 
}
    
// END FILE

