// TEST FILE FOR SHT20 HUMIDITY SENSOR
// https://www.dfrobot.com/product-1636.html tester for this humidity sensor
// Uses I2C communication, checks CRC, and converts I2C bytes into readable floats
// Wiring:
// | SHT20 Wire | Arduino Pin |
// ----------------------------
// | Blue        | SDA1       |
// | Yellow      | SCL1       |
// | Red         | V3.3       |
// | Green       | GND        |
// 
// Seems to be working, for both humidity and temperature data, as of 8/2/2021, on an Arduino MEGA


#include <Wire.h>

#define SLAVE_ADDRESS                         0x40
#define ERROR_I2C_TIMEOUT                     998
#define ERROR_BAD_CRC                         999
#define MAX_WAIT                              100
#define DELAY_INTERVAL                        10
#define SHIFTED_DIVISOR                       0x988000
#define MAX_COUNTER                           (MAX_WAIT/DELAY_INTERVAL)
#define TRIGGER_TEMP_MEASURE_HOLD             0xE3
#define TRIGGER_HUMD_MEASURE_HOLD             0xE5
#define TRIGGER_TEMP_MEASURE_NOHOLD           0xF3
#define TRIGGER_HUMD_MEASURE_NOHOLD           0xF5
#define READ_USER_REG                         0xE7

TwoWire w;

byte readUserRegister()
{
    byte userRegister;
    w.beginTransmission(SLAVE_ADDRESS);
    w.write(READ_USER_REG);
    w.endTransmission();
    w.requestFrom(SLAVE_ADDRESS, 1);
    userRegister = w.read();
    return (userRegister);
}

byte checkCRC(uint16_t message_from_sensor, uint8_t check_value_from_sensor)
{
    uint32_t remainder = (uint32_t)message_from_sensor << 8;
    remainder |= check_value_from_sensor;
    uint32_t divsor = (uint32_t)SHIFTED_DIVISOR;
    for(int i = 0 ; i < 16 ; i++){
        if(remainder & (uint32_t)1 << (23 - i)){
            remainder ^= divsor;
        }
        divsor >>= 1;
    }
    return (byte)remainder;
}

uint16_t readValue(byte cmd)
{
    w.beginTransmission(SLAVE_ADDRESS);
    w.write(cmd);
    w.endTransmission();
    byte toRead;
    byte counter;
    for(counter = 0, toRead = 0 ; counter < MAX_COUNTER && toRead != 3; counter++){
        delay(DELAY_INTERVAL);
        toRead = w.requestFrom(SLAVE_ADDRESS, 3);
    }
    if(counter == MAX_COUNTER){
        return (ERROR_I2C_TIMEOUT);
    }
    byte msb, lsb, checksum;
    msb = w.read();
    lsb = w.read();
    checksum = w.read();
    uint16_t rawValue = ((uint16_t) msb << 8) | (uint16_t) lsb;
    if(checkCRC(rawValue, checksum) != 0){
        return (ERROR_BAD_CRC);
    }
    return rawValue & 0xFFFC;
}

float readHumidity()
{
    uint16_t rawHumidity = readValue(TRIGGER_HUMD_MEASURE_NOHOLD);
    if(rawHumidity == ERROR_I2C_TIMEOUT || rawHumidity == ERROR_BAD_CRC){
        return(rawHumidity);
    }
    float tempRH = rawHumidity * (125.0 / 65536.0);
    float rh = tempRH - 6.0;
    return (rh);
}


float readTemperature()
{
    uint16_t rawTemperature = readValue(TRIGGER_TEMP_MEASURE_NOHOLD);
    if(rawTemperature == ERROR_I2C_TIMEOUT || rawTemperature == ERROR_BAD_CRC){
        return(rawTemperature);
    }
    float tempTemperature = rawTemperature * (175.72 / 65536.0);
    float realTemperature = tempTemperature - 46.85;
    return (realTemperature);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("SHT20 Example!");
  w = Wire;
  w.begin();
  readUserRegister();
}

void loop() {
  // put your main code here, to run repeatedly:
  w.beginTransmission(SLAVE_ADDRESS);
  Serial.println(readTemperature());
}
