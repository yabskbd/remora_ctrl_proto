/*
 * demo: CAN-BUS Shield, receive all frames and print all fields id/type/data
 * to receive frame fastly, a poll in loop() is required.
 *
 * Copyright (C) 2020 Seeed Technology Co.,Ltd.
 */
#include <SPI.h>

#define CAN_2515
// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;
//TODOconst int SPI_CS_PIN2 = 9;
const int CAN_INT_PIN = 2;
const int interruption = 3;
const int interruption2 = 2;
bool toggle = 0;

#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
//TODOmcp2515_can CAN2(SPI_CS_PIN2); // Set CS pin
#define MAX_DATA_SIZE 8

void setup() {
    SERIAL_PORT_MONITOR.begin(115200);
    while (!SERIAL_PORT_MONITOR) {}
    Serial.println("Serial Running!");  
    
    /*TODO
    cli();
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0
    // set compare match register for 1hz increments
    OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler
    TCCR1B |= (1 << CS12) | (1 << CS10);  
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);

    sei();

    */

    /*TODO
    pinMode(interruption, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruption), scream, RISING);
    pinMode(interruption2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruption2), scream2, FALLING);
    */

    #if MAX_DATA_SIZE > 8
    /*
     * To compatible with MCP2515 API,
     * default mode is CAN_CLASSIC_MODE
     * Now set to CANFD mode.
     */
    CAN.setMode(CAN_NORMAL_MODE);
    //TODOCAN2.setMode(CAN_NORMAL_MODE);
    #endif

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {             // init can bus : baudrate = 500k
        SERIAL_PORT_MONITOR.println(F("CAN init fail, retry..."));
        delay(100);
    }
    /*TODO
    while (CAN_OK != CAN2.begin(CAN_500KBPS)) {             // init can bus : baudrate = 500k
        SERIAL_PORT_MONITOR.println(F("CAN2 init fail, retry..."));
        delay(100);
    }
    */
    SERIAL_PORT_MONITOR.println(F("CAN init ok!"));
}

uint32_t id;
uint8_t  type; // bit0: ext, bit1: rtr
uint8_t  len;
byte cdata[MAX_DATA_SIZE] = {0};

void loop() {

  Serial.println("Loop!!!");

  ///////////////////////////////////////////////////
  //CAN
  char prbuf[32 + MAX_DATA_SIZE * 3];
  int i, n;
  unsigned long t;
  static const byte type2[] = {0x00, 0x02, 0x30, 0x32};
  
    // check if data coming
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
        t = millis();
    // read data, len: data length, buf: data buf
    CAN.readMsgBuf(&len, cdata);

    id = CAN.getCanId();
    type = (CAN.isExtendedFrame() << 0) |
           (CAN.isRemoteRequest() << 1);

           
    /*
     * MCP2515(or this driver) could not handle properly
     * the data carried by remote frame
     */

    n = sprintf(prbuf, "%04lu.%03d ", t / 1000, int(t % 1000));
    /* Displayed type:
     *
     * 0x00: standard data frame
     * 0x02: extended data frame
     * 0x30: standard remote frame
     * 0x32: extended remote frame
     */
    
    n += sprintf(prbuf + n, "RX: [%08lX](%02X) ", (unsigned long)id, type2[type]);
    // n += sprintf(prbuf, "RX: [%08lX](%02X) ", id, type);

    for (i = 0; i < len; i++) {
        n += sprintf(prbuf + n, "%02X ", cdata[i]);
    }
    SERIAL_PORT_MONITOR.println(prbuf);
    }

    ///////////////////////////////////////////
    //CAN2

    /*TODO

    // check if data coming
    if (CAN_MSGAVAIL == CAN2.checkReceive()) {
        t = millis();
    // read data, len: data length, buf: data buf
    CAN2.readMsgBuf(&len, cdata);

    id = CAN2.getCanId();
    type = (CAN2.isExtendedFrame() << 0) |
           (CAN2.isRemoteRequest() << 1);
    
     //* MCP2515(or this driver) could not handle properly
     //* the data carried by remote frame

    n = sprintf(prbuf, "%04lu.%03d ", t / 1000, int(t % 1000));
    //Displayed type:
    // *
    // * 0x00: standard data frame
    // * 0x02: extended data frame
    // * 0x30: standard remote frame
    // * 0x32: extended remote frame
     
    n += sprintf(prbuf + n, "RX: [%08lX](%02X) ", (unsigned long)id, type2[type]);
    // n += sprintf(prbuf, "RX: [%08lX](%02X) ", id, type);

    for (i = 0; i < len; i++) {
        n += sprintf(prbuf + n, "%02X ", cdata[i]);
    }
    SERIAL_PORT_MONITOR.print("CAN2: ");
    SERIAL_PORT_MONITOR.println(prbuf);
    }

    */

    /////////////////////////////////////////////
}

/*TODO

void scream() {
  Serial.println("INTERRUPTED");
}
void scream2() {
  Serial.println("INTERRUPTED2");
}

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  if (toggle){
    Serial.println("Time");
  }
  else{
    Serial.println("Not Time");
  }
  toggle = !toggle;
}

*/

// END FILE