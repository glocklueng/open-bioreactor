// http://tushev.org/articles/electronics/42-how-it-works-ds18b20-and-arduino
// http://datasheets.maxim-ic.com/en/ds/DS18B20.pdf
// http://www.milesburton.com/?title=Dallas_Temperature_Control_Library#Latest

/*
For temperature detection we will use Dallas DS18B20
 It is a 1-wire component that gives the temperature in Celcius without any callibration.
 The resolution may be 9, 10, 11 or 12 bits
 All the sensor have a unique ID so that theoretically we can put many temperature sensor on the same bus
 One problem is that we need to send a message to ask to calculate the temperature and if we use 12 bits
 resolution it takes around 700mS. We should definitely not just "delay" and block the processor.
 The idea is to send the instruction to calculate the temperature and laster to retrieve the result.
 If we don't have many 1-wire component on the same bus we can just ask to "ALL" to retrieve the temperature 
 and therefore we don't need to discover the unique ID of all the component.
 */

/*
In the DallasTemperature library we have some important features
 * setWaitForConversion
 * requestTemperatures
 * getTempC
 */

#define ONE_WIRE_BUS_A 6
#define ONE_WIRE_BUS_B 7

OneWire oneWireA(ONE_WIRE_BUS_A);
OneWire oneWireB(ONE_WIRE_BUS_B);
DallasTemperature sensorA(&oneWireA);
DallasTemperature sensorB(&oneWireB);
DeviceAddress deviceAddressA;
DeviceAddress deviceAddressB;
float temperatureA;
float temperatureB;
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

float targetTemperature;


void setupTemperature() {
  // during setup we will create the connection
  sensorA.begin();
  sensorA.setWaitForConversion(false);
  sensorB.begin();
  sensorB.setWaitForConversion(false);
  sensorA.getAddress(deviceAddressA, 0);
  sensorB.getAddress(deviceAddressB, 0); 

  timer.setInterval(180000, requestTemperature);
  if (DEBUG) {
    timer.setInterval(180000, displayTemperature);
  }

  sensorA.setResolution(deviceAddressA, 12);
  sensorB.setResolution(deviceAddressB, 12);

  if (DEBUG) {
    getAllSensorsStatus(&Serial);

  }
}


void requestTemperature() {
  int start=millis();
  sensorA.requestTemperatures(); // Send the command to get temperatures
  sensorB.requestTemperatures(); // Send the command to get temperatures
  timer.setTimeout(700, getTemperature); // we wait 700 ms to update the temperature, 700 mS for 12 bits is enough
  int last=millis()-start;
  if (DEBUG) {
    Serial.print("Request temperatures took (ms): ");
    Serial.println(last);
  }
}

void getTemperature() {
  int start=millis();
  temperatureA = sensorA.getTempC(deviceAddressA);
  temperatureB = sensorB.getTempC(deviceAddressB);
  int last=millis()-start;
  if (DEBUG) {
    Serial.print("Get temperatures took (ms): ");
    Serial.println(last);
  }
}

void displayTemperature() {
  Serial.print("Temperatures: ");
  Serial.print(temperatureA);
  Serial.print(" - ");
  Serial.println(temperatureB);
}

void loopTemperature() {

}

void getAllSensorsStatus(Print* output) {
  getSensorStatus(sensorA, ONE_WIRE_BUS_A, output);
  getSensorStatus(sensorB, ONE_WIRE_BUS_B, output);
}

void getSensorStatus(DallasTemperature sensors, int bus, Print* output) {
  output->print("Locating temperature devices on bus: ");
  output->print(bus);
  output->print(". Found ");
  output->print(sensors.getDeviceCount(), DEC);
  output->println(" devices.");
  output->print("Wait for Conversion: ");
  output->println(sensors.getWaitForConversion());
  output->print("Resolution: ");
  output->println(sensors.getResolution());
  int numberOfDevices = sensors.getDeviceCount();
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
    {
      output->print("Found device ");
      output->print(i, DEC);
      output->print(" with address: ");
      printAddress(tempDeviceAddress, output);
      // The temperature is not updated here !!!
      output->print(" - Temperature: ");
      output->println( sensors.getTempC(tempDeviceAddress));
    } 
    else {
      output->print("Found ghost device at ");
      output->print(i, DEC);
      output->print(" but could not detect address. Check power and cabling");
    }
  }
}


// function to print a device address
void printAddress(DeviceAddress deviceAddress, Print* output)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) output->print("0");
    output->print(deviceAddress[i], HEX);
    output->print(" ");
  }
}


void setTargetTemperature(Client client, String parameters) {
  // BUG in atof ???? We loose the last digit
  // We just add a "." to fix this problem
  parameters=parameters+".";
  int len=parameters.length();
  char strVal[len];
  parameters.toCharArray(strVal, len);
  targetTemperature=atof(strVal);
  EXROM.write(EE_TARGET_TEMP, targetTemperature);
  float savedTemperature;
  EXROM.read(EE_TARGET_TEMP, &savedTemperature);
  httpHeader(client, 200,"text/html");
  client.println("Temperature saved in EEPROM: ");
  client.println(savedTemperature);
}



