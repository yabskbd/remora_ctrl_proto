/****************************************************************************
 * Requeset C02 Sensor for C02 concentration data over serail
 * Convert received data to PPM and sends over CAN BUS with extended CAN 
 * with 250 KBPS Baud Rate. 
 * This is intended to run on Arudino Nano 
 * 
 * Note for debuging please define macro CO2_SENSOR_CAN_ENABLE_SERIAL_PRINT 
 * to enable Serial Print. For production code this will be undef
 * 
 * 
 *****************************************************************************/

#include <SPI.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "mcp2515_can.h"

/*! Declare General Macros */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/*! SPI pin for CAN Controller Chip */
#define CO2_SENSOR_CAN_SPI_CS_PIN  9 
/*!  Query CO2 Sensor Every 1 seconds. units in milliseconds */
#define CO2_SENSOR_CAN_LOOP_DELAY  1000 
/*! CAN BAUD RATE.
    IMPORTANT: The Hardware for this operates at 250kbps when set to the below MACRO */
#define CO2_SENSOR_CAN_BAUD_RATE CAN_500KBPS
/* Use Extened CAN ID (29bit). This Macro is passed to send api */
#define CO2_SENSOR_CAN_USE_EXTENDED_IDS    TRUE

/* CAN ID: J1939 PGN 65320 = 0xFF2800 */
#define CO2_SENSOR_CHANGE_CAN_ID           0xFF28FF
#define CO2_SENSOR_CHANGE_CONV_PPM_ID      0xFF28FE

#define CO2_SENSOR_CHANGE_CAN_ID_ACK       0xFF287F
#define CO2_SENSOR_CHANGE_CONV_PPM_ID_ACK  0xFF287E

/* PPM to percent:
 x(%) = x(ppm) / 10,000
 
 Ambient C02 PPM
   200 - 400 PPM
 */


/*! MAX CAN Data Bytes. Defined by spec */
#define CO2_SENSOR_CAN_MAX_CAN_DATA_BYTE 8

/*! Software Serial Setup RX_PIN, TX_PIN */
#define CO2_SENSOR_CAN_SW_SERIAL_RX_PIN 2
#define CO2_SENSOR_CAN_SW_SERIAL_TX_PIN 3

/*! Define for debuging to enable Serail Print
  IMPORTANT: undef when in production code */
#define CO2_SENSOR_CAN_ENABLE_SERIAL_PRINT 

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

struct co2_sensor_can_eeprom_crc_s
{
  uint32_t cookie;
  uint32_t signature;
  uint32_t crc;
};

#define EEPROM_MAX_ADDR  (EEPROM.length() - 1)
#define EEPROM_CAN_ID_ADDR   0 
#define EEPROM_PPM_CONV_ADDR     EEPROM_CAN_ID_ADDR + sizeof(uint32_t)
#define COOKIE         0xDB9FD74D
#define SIGNATURE      0x8B772C32
#define CRC            (COOKIE ^ SIGNATURE)

#define CRC_BYTE_SIZE sizeof(co2_sensor_can_eeprom_crc_s)
#define CRC_ADDRS     (EEPROM_MAX_ADDR - CRC_BYTE_SIZE)


/*! Globals */
/*! Software Serial for C02 Sensor Data. RX, TX */
SoftwareSerial co2_sensor_serial(CO2_SENSOR_CAN_SW_SERIAL_RX_PIN, CO2_SENSOR_CAN_SW_SERIAL_TX_PIN); 
/*! Initialze CAN Controller. CAN Set to 250kbps */
mcp2515_can CAN(CO2_SENSOR_CAN_SPI_CS_PIN); 
/*! Conversion multiplier to ppm from CO2 Sensor RX data */
uint32_t co2_sensor_can_ppm_conv = 10;

/*! Default CAN ID */
/* CAN ID: J1939 PGN 65320 = 0xFF2800 */
uint32_t co2_sensor_can_ext_id   = 0xFF2820;

void setup()
{
    /* Init communicates with the CO2 sensor */
    co2_sensor_serial.begin(9600); //Set serial baud rate to 9600 
    Serial.begin(9600);            //Set serial baud rate to 9600

    /* Init Baud Rate with 250kbps */
    while (CAN_OK != CAN.begin(CO2_SENSOR_CAN_BAUD_RATE))
    { 
        CO2_SENSOR_CAN_SERIAL_PRINTLN("CAN init fail, retry...");
        delay(100);
    }
    CO2_SENSOR_CAN_SERIAL_PRINTLN("CAN init ok!");

    Serial.println(EEPROM_MAX_ADDR);
    Serial.println(CRC_BYTE_SIZE);
    Serial.println(CRC_ADDRS);

    co2_sensor_can_eeprom_crc_s check_crc_mem;
    EEPROM.get( CRC_ADDRS, check_crc_mem );
    Serial.println( "Read custom object from EEPROM: " );
    Serial.println( check_crc_mem.cookie );
    Serial.println( check_crc_mem.signature );
    Serial.println( check_crc_mem.crc );
    if( ((check_crc_mem.cookie ^ check_crc_mem.signature) !=  check_crc_mem.crc) || 
        (check_crc_mem.cookie != COOKIE))
    {
        /* Initialize EEPROM */
        Serial.println( "Init EEPROM" );
        co2_sensor_can_eeprom_crc_s init_crc_write;
        init_crc_write.cookie = COOKIE;
        init_crc_write.signature = SIGNATURE;
        init_crc_write.crc = CRC;
        EEPROM.put(CRC_ADDRS, init_crc_write);

        EEPROM.put(EEPROM_CAN_ID_ADDR, co2_sensor_can_ext_id);
        EEPROM.put(EEPROM_PPM_CONV_ADDR, co2_sensor_can_ppm_conv);
    }
    else
    {
        /* GET */
		Serial.println( "GET EEPROM" );
		EEPROM.get(EEPROM_CAN_ID_ADDR, co2_sensor_can_ext_id);
		EEPROM.get(EEPROM_PPM_CONV_ADDR, co2_sensor_can_ppm_conv);
    }
}

void loop() {
 
    String co2_sensor_serial_rx;   
    
    /* Request CO2 Sensor for CO2 Concentration Data */
    co2_sensor_serial.write("Z\r\n");
	/* Query Mulitplier */
	//co2_sensor_serial.write(".\r\n");
    if (co2_sensor_serial.available() > 0) {
        /* Read avail incoming byte */
        co2_sensor_serial_rx = co2_sensor_serial.readString();
        co2_sensor_serial_rx.trim();
        /* Debug Print for RX data from CO2 Serial */
        CO2_SENSOR_CAN_SERIAL_PRINT("Trimmed rx: ");
        CO2_SENSOR_CAN_SERIAL_PRINTLN(co2_sensor_serial_rx);
        /* Rx Data ex:  'Z 00382'
           grabs the last 5 digits of the returned value
           drops the " Z " or similar prefix
           only keeps the value in decimal form */ 
        String incomingByteShort = co2_sensor_serial_rx.substring(co2_sensor_serial_rx.length() - 5);
        CO2_SENSOR_CAN_SERIAL_PRINT("Final String for INT: ");
        CO2_SENSOR_CAN_SERIAL_PRINTLN(incomingByteShort);
        /* Convert RX string to unsinged int */
        uint32_t co2_ppm = (uint32_t)incomingByteShort.toInt();
        /* Convert Serail RX data to PPM */
        /* Debug Print */
        co2_ppm = co2_ppm * co2_sensor_can_ppm_conv;
        CO2_SENSOR_CAN_SERIAL_PRINT("co2_ppm ");
        CO2_SENSOR_CAN_SERIAL_PRINTLN(co2_ppm);
        CO2_SENSOR_CAN_SERIAL_PRINT("size of co2_ppm ");
        CO2_SENSOR_CAN_SERIAL_PRINTLN(sizeof(co2_ppm));
		
		/* Send Data Over CANBus with the proper ID and Formated Data s */
		send_can_msg(co2_sensor_can_ext_id, co2_ppm);
		            
    } /* End of Sensor Serial Avalibale Check */

    unsigned char len = 0;
    unsigned char can_rx_buf[8];   

    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
        CAN.readMsgBuf(&len, can_rx_buf);    // read data,  len: data length, buf: data buf

        unsigned long canId = CAN.getCanId();

        CO2_SENSOR_CAN_SERIAL_PRINTLN("-----------------------------");
        CO2_SENSOR_CAN_SERIAL_PRINT("Get data from ID: 0x");
        CO2_SENSOR_CAN_SERIAL_PRINT_EXT(canId, HEX);
        CO2_SENSOR_CAN_SERIAL_PRINT("\t");
        for (int i = 0; i < len; i++) { // print the data
            CO2_SENSOR_CAN_SERIAL_PRINT_EXT(can_rx_buf[i], HEX);
            CO2_SENSOR_CAN_SERIAL_PRINT("\t");
        }
		CO2_SENSOR_CAN_SERIAL_PRINTLN();
        
		if(canId == CO2_SENSOR_CHANGE_CAN_ID)
        {
			uint32_t new_can_id = 0; 
			new_can_id = unpack_left_shift_u32(can_rx_buf[3], 24u, 0xffu);
			new_can_id |= unpack_left_shift_u32(can_rx_buf[2], 16u, 0xffu);
			new_can_id |= unpack_left_shift_u32(can_rx_buf[1], 8u, 0xffu);
			new_can_id |= unpack_left_shift_u32(can_rx_buf[0], 0u, 0xffu);
			
			CO2_SENSOR_CAN_SERIAL_PRINT_EXT(new_can_id, HEX);
			CO2_SENSOR_CAN_SERIAL_PRINTLN();
			EEPROM.put(EEPROM_CAN_ID_ADDR, new_can_id);
			/* Send ACK */
			send_can_msg(CO2_SENSOR_CHANGE_CAN_ID_ACK, new_can_id);
			/* Set Current ID to New ID*/
			co2_sensor_can_ext_id = new_can_id;
        }
		else if(canId == CO2_SENSOR_CHANGE_CONV_PPM_ID)
		{
            /* Conv PPM must be in the first 2 Bytes in Little Endian format*/
			uint32_t new_conv_ppm = 0;
			new_conv_ppm |= unpack_left_shift_u32(can_rx_buf[1], 8u, 0xffu);
			new_conv_ppm |= unpack_left_shift_u32(can_rx_buf[0], 0u, 0xffu);
			
			CO2_SENSOR_CAN_SERIAL_PRINT("New PPM Conv value: ");
			CO2_SENSOR_CAN_SERIAL_PRINTLN(new_conv_ppm);
			if(new_conv_ppm != 0)
			{
				EEPROM.put(EEPROM_PPM_CONV_ADDR, new_conv_ppm);
				co2_sensor_can_ppm_conv = new_conv_ppm;
				/* Send ACK */
				send_can_msg(CO2_SENSOR_CHANGE_CONV_PPM_ID_ACK, new_conv_ppm);
			}
			else
			{
                /* first 2 bytes are zero then send out current multipler over CAN */
				/* Send Current Value */
				send_can_msg(CO2_SENSOR_CHANGE_CONV_PPM_ID_ACK, co2_sensor_can_ppm_conv);
			}

		}

    }

    /* Loop Dealy, to query CO2 Sensor Data Every X Seconds */
    delay(CO2_SENSOR_CAN_LOOP_DELAY); 
}

uint32_t unpack_left_shift_u32(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint32_t)((uint32_t)(value & mask) << shift);
}


void send_can_msg (uint32_t can_id, uint32_t data)
{
	/* CAN Data TX Buffer. init to all zero for each txs */
    unsigned char can_tx_buf[CO2_SENSOR_CAN_MAX_CAN_DATA_BYTE] = {0, 0, 0, 0, 0, 0, 0, 0};
	/* moves data from uint32_t to char array to be TX over CAN */ 
	memcpy(can_tx_buf, &data, sizeof(uint32_t)); 
	/* Send CAN Data to Bus. 
	   IMPORTANT: Data is sent as Little Endian over CAN 
	   The order is LSB followed by MSB.
	   i.e 3750 ppm will be [A6, 0E] 
	   which will be 0x0EA6 */
	CAN.sendMsgBuf(can_id, CO2_SENSOR_CAN_USE_EXTENDED_IDS, 
				   CO2_SENSOR_CAN_MAX_CAN_DATA_BYTE, can_tx_buf); 
	CO2_SENSOR_CAN_SERIAL_PRINTLN("CAN BUS sendMsgBuf ok!");
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