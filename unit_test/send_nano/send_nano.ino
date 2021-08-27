//sends a CAN signal from a Nano 33 BLE SENSE which is getting data from a CO2 sensor

// demo: CAN-BUS Shield, send data
// loovee@seeed.cc


#include <SPI.h>

#define CAN_2515
// #define CAN_2518FD

// Set SPI CS Pin according to your hardware

#if defined(SEEED_WIO_TERMINAL) && defined(CAN_2518FD)
// For Wio Terminal w/ MCP2518FD RPi Hat：
// Channel 0 SPI_CS Pin: BCM 8
// Channel 1 SPI_CS Pin: BCM 7
// Interupt Pin: BCM25
const int SPI_CS_PIN  = BCM8;
const int CAN_INT_PIN = BCM25;
#else

// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;
#endif


#ifdef CAN_2518FD
#include "mcp2518fd_can.h"
mcp2518fd CAN(SPI_CS_PIN); // Set CS pin
#endif

#ifdef CAN_2515
#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
#endif

void setup() {
    //pinMode(2, INPUT);
    //pinMode(3, OUTPUT);
    Serial1.begin(9600);             //Set serial baud rate to 9600
    Serial.begin(9600);             //Set serial baud rate to 9600
    while(!Serial){};

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {             // init can bus : baudrate = 250k
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
        delay(100);
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}

unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
String incomingByte; // for incoming serial data
void loop() {
    Serial1.write("Z\r\n");
    //Serial.println("Start");
    if (Serial1.available() > 0) {
    // read the incoming byte:
    //incomingByte = Serial.readStringUntil('\n');
    incomingByte = Serial1.readString();
    // say what you got:
    Serial.print("rx: ");
    Serial.println(incomingByte);
  }
    delay(1000);                        //1 second delay
    //grabs the last 5 digits of the returned value
    //drops the " Z " or similar prefix
    //only keeps the value in decimal form
    String incomingByteShort = incomingByte.substring(incomingByte.length()-5);
    int cdata = incomingByteShort.toInt();
    SERIAL_PORT_MONITOR.print("cdata ");
    SERIAL_PORT_MONITOR.println(cdata);
    SERIAL_PORT_MONITOR.print("size of cdata ");
    SERIAL_PORT_MONITOR.println(sizeof(cdata));
    memcpy(stmp, &cdata, 4);
    for (int i = 0; i < 8; i++) { // print the data
            SERIAL_PORT_MONITOR.print(stmp[i], HEX);
            SERIAL_PORT_MONITOR.print("\t");
        }
    CAN.sendMsgBuf(0x00, 0, 8, stmp);
    delay(100);                       // send data per 100ms
    SERIAL_PORT_MONITOR.println("CAN BUS sendMsgBuf ok!");
    for (int i = 0; i < 8; i++) {
        stmp[i] = 0;
    }
}

// END FILE