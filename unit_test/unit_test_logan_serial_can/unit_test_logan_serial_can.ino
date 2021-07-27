// RECV EXAMPLE OF SERIAL CAN MODULE
// unsigned char recv(unsigned long *id, uchar *buf);
// SUPPORT: joney.sui@longan-labs.cc
#include <Serial_CAN_Module.h>
#include <SoftwareSerial.h>

Serial_CAN can;

#define can_tx  10           // tx of serial can module connect to D2
#define can_rx  11           // rx of serial can module connect to D3

void setup()
{
  PCICR |= B00000100;      //Bit2 = 1 -> "PCIE0" enabeled (PCINT0 to PCINT7)
  PCMSK2 |= B10000000;      //Bit5 = 1 -> "PCINT8" enabeled -> A15 will trigger interrupt


    pinMode(A15, INPUT_PULLUP);    // sets the digital pin 13 as output
    Serial.begin(9600);

    can.begin(can_tx, can_rx, 9600);      // tx, rx
    if(can.canRate(CAN_RATE_500))
    {
        Serial.println("set can rate ok");
    }
    else
    {
        Serial.println("set can rate fail");
    }
    Serial.println("begin");
}

unsigned long id = 0;
unsigned char dta[8];

// send(unsigned long id, byte ext, byte rtrBit, byte len, const byte *buf);
void loop()
{
   
}

ISR (PCINT0_vect) 
{

   //Serial.println("Top of Loop");
    if(can.recv(&id, dta))
    {
        Serial.print("GET DATA FROM ID: ");
        Serial.println(id, HEX);
        for(int i=0; i<8; i++)
        {
            Serial.print("0x");
            Serial.print(dta[i], HEX);
            Serial.print('\t');
        }
        Serial.println();
    }
} 


// END FILE
