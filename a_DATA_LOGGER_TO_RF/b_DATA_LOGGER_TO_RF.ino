//contains the implementation of the DataLogger class
//most important features are the implementation and definition of an sensorType and subindex
//sensorType is roughly to the ID of a sensor type
//subIndex is roughly the number of the exact sensor within all the sensors of the same type
//one sensorType would point to SHT10 implementation, while subIndex point to an actual sensor and its position
// the second thermocouple would be index of 0, subIndex of 1

//there is also a more complicated way to do this via inheritance

//creates an instance of the class
DataLogger::DataLogger(int nKType, int nPr30Sen, int nSHT20, int nSHT10, int nSEN0220, int initialStage, bool preheatInput) {
  this->numKType = nKType;
  indexTracker[0] = nKType;
  this->numPr30Sen = nPr30Sen;
  indexTracker[1] = nPr30Sen;
  this->numPr2kSen = 0; //implement if needed
  indexTracker[2] = 0;
  this->numSHT20 = nSHT20;
  indexTracker[3] = nSHT20; //humidity
  indexTracker[4] = nSHT20; //temperature
  indexTracker[5] = nSHT20; //dew point
  this->numSHT10 = nSHT10;
  indexTracker[6] = nSHT10; //humidity
  indexTracker[7] = nSHT10; //temperature
  indexTracker[8] = nSHT10; //dew point
  this->numSEN0220 = nSEN0220;
  indexTracker[9] = nSEN0220;
  this->stage = initialStage;
  this->preheat = preheatInput;

  this->numSensors = psumIndexTracker(10);
}

int DataLogger::psumIndexTracker(int index) {
  int sum = 0;

  for(int i = 0; i < index; i++) {
    sum += indexTracker[i];
  }

  return sum;
}

//return false if failure to initialize
bool DataLogger::initialize() {
  //init RTC
  if (!initRTC()) {
    return false;
  }

  //init cycleData array
  for (int i = 0; i < numSensors; i++) {
    cycleData[i] = 0.0;
  }

  //init sensors
  if (!initSensors()) {
    return false;
  }

  Serial.println("inited sensors");

  pinMode(fanPin, OUTPUT);
  pinMode(heaterPin, OUTPUT);

  return true;
}

bool DataLogger::initRTC() {
  Serial.println("Initializing RTC...");

  int initTime = 10;

  for (int i = 0; i < initTime; i++) {
    Serial.print(initTime - i);
    Serial.println(" second(s) remain for RTC initialization");
    delay(1000);
  }

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.println("keep going even though it's a bad idea");
    //Serial.flush();
    //abort();
  }

  if (rtc.now().year() > 2100) {
    Serial.println("RTC isn't communicating properly.");
    Serial.println("keep going even though it's a bad idea");
    //return false;
  }

  return true;
}

bool DataLogger::initSensors() {
  //initialize the thermocouples
  for (int i = 0; i < numKType; i++) {
    while (!thermocouples[i].begin()) {
      delay(50);
    }
  }

  //no need to initialize the pressure sensors

  //init SHT20
  Wire.begin();
  sht20.begin();

  //init SEN0220
  initSEN0220();

  Serial.println("done preheating");

  return true;
}

bool DataLogger::initSEN0220() {
  /*
  Serial.println(numKType);
  Serial.println(numPr30Sen);
  Serial.println(numSensors);
  Serial.println(preheat);
  */
  if (!preheat) {
    Serial.println("Opted for no preheating.");
    return true;
  }
  for (int i = 0; i < preheatTime; i++) {
    Serial.println("Pre-heating in progress.");
    Serial.print(preheatTime - i);
    Serial.println(" second(s) remain.");
    delay(1000);
  }

  return true;
}

//looks at each sensor and records a sensor value
//sends to a string (cycleOutput) for printing and an array (cycleData) for storage
void DataLogger::sweep() {
  Serial.print("Number of sensors: ");
  Serial.println(numSensors); 
  for (int i = 0; i < numSensors; i++) {
    double val = getSensorVal(i);
    cycleData[i] = val;
  }

}

double DataLogger::getSensorVal(int index) {
  delay(5); //this prevents the sensors from being skipped (led to the output "0" error from the thermocouples)

  int sensorType = indexToSensorType(index); //TODO convert to byte
  int sensorSubIndex = indexToSensorSubIndex(index);

  switch(sensorType) {
    case 0: //thermocouple
      return getThermoVal(sensorSubIndex);
      break;
    case 1: //30psi pressure sensor
      return getPrVal(sensorSubIndex, 30);
      break;
    case 2: //2,000psi pressure sensor
      return getPrVal(sensorSubIndex, 2000);
      break;
    case 3: //SHT humidity
      return getSHT20Humidity(sensorSubIndex);
      break;
    case 4: //SHT20 temperature
      return getSHT20Temperature(sensorSubIndex);
      break;
    case 5: //SHT20 dew point
      return getSHT20DewPoint(sensorSubIndex);
      break;
    case 6: //SHT10 humidity
      return getSHT10Humidity(sensorSubIndex);
      break;
    case 7: //SHT10 humidity
      return getSHT10Temperature(sensorSubIndex);
      break;
    case 8: //SHT10 dew point
      return getSHT10DewPoint(sensorSubIndex);
      break;
    case 9: //get SEN0220 carbon in ppm
      return getSEN0220Val(sensorSubIndex);
      break;
    default:
      return -1;
      break;
  }

}

double DataLogger::getThermoVal(int subIndex) {
  return thermocouples[subIndex].readCelsius();
}

//this could be made more generalized
double DataLogger::getPrVal(int subIndex, int maxVal) {
  double input = analogRead(matchAnalogPinToID[subIndex]);
  input = input * 5.0 / 1023.0; //in volts
  if (input < 0.4 || input > 4.6) {
    return -1.0;
  }
  input = input - 0.5;
  return (maxVal * input / 4.0);
}

double DataLogger::getSHT20Humidity(int subIndex) {
  sht20.measure_all();
  return sht20.RH;
}

double DataLogger::getSHT20Temperature(int subIndex) {
  sht20.measure_all();
  return sht20.tempC;
}

//I'm having trouble finding the implementation of the .dew_pointC functionality
double DataLogger::getSHT20DewPoint(int subIndex) {
  sht20.measure_all();
  return sht20.dew_pointC;
}

double DataLogger::getSHT10Humidity(int subIndex) {
  return sht10s[subIndex].readHumidity();
}

double DataLogger::getSHT10Temperature(int subIndex) {
  return sht10s[subIndex].readTemperatureC();
}

//dew point calculation from here https://iridl.ldeo.columbia.edu/dochelp/QA/Basic/dewpoint.html
//only accurate at high humidity (within 5 degC of dew point)
double DataLogger::getSHT10DewPoint(int subIndex) {
  double t = sht10s[subIndex].readTemperatureC();
  double rh = sht10s[subIndex].readHumidity();
  double dp = t - ((100.0-rh)/5.0);
  Serial.print("dew point is: ");
  Serial.println(dp);
  return dp;
}

double DataLogger::getSEN0220Val(int subIndex) {
  Serial2.write(hexdata, 9);
  delay(500);

  for (int i = 0, j = 0; i < 9; i++)
  {
    //Serial.println("Getting Data");

    if (Serial2.available() > 0)
    {
      long hi, lo, CO2;
      int ch = Serial2.read();

      if (i == 2) {
        hi = ch;    //High concentration
      }
      if (i == 3) {
        lo = ch;    //Low concentration
      }
      if (i == 8) {
        CO2 = hi * 256 + lo; //CO2 concentration
        Serial.print("CO2 concentration: ");
        Serial.print(CO2);
        Serial.println("ppm");
        return CO2;
      }
    }
  }

  return -2.0;

}

double DataLogger::getSEN0220Error(double value) {
  return 50 + (0.05 * value);
}

//retrieves and formats the RTC value as a string
//hopefully this works
//format: `YYYYoMMdDDhHHmMMsSS`
String DataLogger::getRTCValue() {
  DateTime here = rtc.now();
  String OO = String(here.month());
  String DD = String(here.day());
  String HH = String(here.hour());
  String MM = String(here.minute());
  String SS = String(here.second());

  OO = makeTwoDigits(OO);
  DD = makeTwoDigits(DD);
  HH = makeTwoDigits(HH);
  MM = makeTwoDigits(MM);
  SS = makeTwoDigits(SS);

  String data = String(here.year());
  data += "o";
  data += OO;
  data += "d";
  data += DD;
  data += "h";
  data += HH;
  data += "m";
  data += MM;
  data += "s";
  data += SS;

  Serial.print("the getRTCValue is ");
  Serial.println(data);

  return data;
}

//converts 1 digits by adding a 0 to the front
//leaves 2 digits the same
String DataLogger::makeTwoDigits(String input) {
  String out = input;
  if (out.length() == 1) {
    out = "0";
    out += input;
  }

  return out;
}

//includes RTC value and time since start
//RTC time is always slightly off from timing of recording
String DataLogger::getCycleDataAsString() {
  String push = "";

  //add date and time to start of string
  push = getRTCValue();
  push += ",";

  //add time in milliseconds to start of string
  push = String(millis());
  push += ",";

  for(int i = 0; i < numSensors; i++) {
    push += cycleData[i];
    if(i < numSensors - 1) {
      push += ",";
    }
  }

  return push;
}

void DataLogger::setFan(bool effect) {
  if(effect) {
    digitalWrite(fanPin, HIGH);
  }
  else {
    digitalWrite(fanPin, LOW);
  }
}

void DataLogger::setHeater(bool effect) {
  if(effect) {
    digitalWrite(heaterPin, HIGH);
  }
  else {
    digitalWrite(heaterPin, LOW);
  }
}

//finds the type of sensor
//refer to getSensorVal() function above for the pairings between the number and the sensor implementation
int DataLogger::indexToSensorType(int index) {
  int sensorIter = 0;
  for(int i = 0; i < 10; i++) {
    sensorIter += indexTracker[i];
    if(sensorIter > index) {
      return i;
    }
  }

  return -1;
}

//finds the number of the sensor within the list of sensors of that type
int DataLogger::indexToSensorSubIndex(int index) {
  int sensorIter = 0;
  for(int i = 0; i < 10; i++) {
    sensorIter += indexTracker[i];
    if(sensorIter > index) {
      return index - (sensorIter - indexTracker[i]);
    }
  }

  return -2;
}

void DataLogger::assess() {
  //TODO
  //do cool stuff like output messages to serial, change stage, or turn on/off heater
  for(int i = 0; i < numSensors; i++) {
    assessSensor(i);
  }
}

//gives qualitative data on sensor state
void DataLogger::assessSensor(int index) {

  switch(indexToSensorType(index)) {
    case 0: //thermocouple
      if(cycleData[index] > 400) {
        Serial.print("Thermocouple ");
        Serial.print(indexToSensorSubIndex(index));
        Serial.println(" is greater than 400 C");
      }
      break;
    case 1: //30psi pressure sensors
      if(cycleData[index] > 25) {
        Serial.print("30 psi Pressure Sensor");
        Serial.print(indexToSensorSubIndex(index));
        Serial.println(" is greater than 25 psi");
      }
      break;
    case 2: //2kpsi pressure sensors
      if(cycleData[index] > 1800) {
        Serial.print("2000 psi Pressure Sensor");
        Serial.print(indexToSensorSubIndex(index));
        Serial.println(" is greater than 1800 psi");
      }
      break;
    case 3: //SHT20 humidity
      if(cycleData[index] > 90) {
        Serial.print("SHT20 Sensor");
        Serial.println(" is above 90 percent humidity");
      }
      break;
    case 4: //SHT20 temperature
      if(cycleData[index] > 400) {
        Serial.print("SHT20 Sensor");
        Serial.println(" is greater than 400 C");
      }
      break;
    case 6: //SHT10 humidity
      if(cycleData[index] > 90) {
        Serial.print("SHT10 Sensor");
        Serial.println(" is above 90 percent humidity");
      }
      break;
    case 7: //SHT10 temperature
      if(cycleData[index] > 400) {
        Serial.print("SHT10 Sensor");
        Serial.println(" is greater than 400 C");
      }
      break;
    case 9: //SEN0220
      if(cycleData[index] > 4500) {
        Serial.println("Approaching Max CO2 concentration");
      }
      break;
  }
}
