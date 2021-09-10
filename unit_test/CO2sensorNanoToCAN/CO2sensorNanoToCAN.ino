//sends a CAN signal from a base-model Arduino Nano which is getting serial data from a CO2 sensor
//data still needs to be multiplied by a factor of 10 through a DBC file

#include <SPI.h>
#include <SoftwareSerial.h>

// the cs pin is pin 9
const int SPI_CS_PIN = 9;

#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin

SoftwareSerial sensorSerial(2, 3); //2 is RX, 3 is TX

void setup()
{
    sensorSerial.begin(9600); //Set serial baud rate to 9600 //communicates with the CO2 sensor
    Serial.begin(9600);       //Set serial baud rate to 9600 //provides debugging info through USB port
    while (!Serial)
    {
    };

    while (CAN_OK != CAN.begin(CAN_500KBPS))
    { // init can bus : baudrate = 250k (CAN_500KBPS achieves and 250k baudrate)
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
        delay(100);
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}

unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //output array on CAN bus
String incomingByte;                              // for incoming serial data
void loop() {
    sensorSerial.write("Z\r\n");
    if (sensorSerial.available() > 0) {
        // read the incoming byte:
        //incomingByte = Serial.readStringUntil('\n');
        incomingByte = sensorSerial.readString();
        // say what you got:
        Serial.print("rx: ");
        Serial.println(incomingByte);
    }
    delay(5000); //1 second delay
    //grabs the last 5 digits of the returned value
    //drops the " Z " or similar prefix
    //only keeps the value in decimal form
    String incomingByteShort = incomingByte.substring(incomingByte.length() - 5);
    uint32_t cdata = (uint32_t)incomingByteShort.toInt();
    SERIAL_PORT_MONITOR.print("cdata ");
    SERIAL_PORT_MONITOR.println(cdata);
    SERIAL_PORT_MONITOR.print("size of cdata ");
    SERIAL_PORT_MONITOR.println(sizeof(cdata));
    memcpy(stmp, &cdata, sizeof(cdata)); //moves data from uint32_t to char array to be printed
    for (int i = 0; i < 8; i++)
    { // print the data
        SERIAL_PORT_MONITOR.print(stmp[i], HEX);
        SERIAL_PORT_MONITOR.print("\t");
    }
    CAN.sendMsgBuf(0x00, 0, 8, stmp); //send data through CAN
    delay(10); //extra delay to aid in printing
    SERIAL_PORT_MONITOR.println("CAN BUS sendMsgBuf ok!");
    for (int i = 0; i < 8; i++)
    {
        stmp[i] = 0;
    }
}

// END FILE