void temperatureSetup();
void temperatureUpdate();
boolean temperatureGetOneWire(byte *addr, float *retnTemp, OneWire * OW);
float temperatureMeasuredInLiquid();
float temperatureMeasuredBottomPlate();
float temperatureMeasuredAmbient();
//void temperatureWriteLogToSDcard();

/*
This code was taken from the Arduino forum (arduino.cc).
 It has been modified to meet the project's needs.
 */


// DS18S20 Temperature chip i/o
OneWire owBottom(PIN_TEMPERATURE_SENSOR_BOTTOM);
OneWire owInLiquid(PIN_TEMPERATURE_SENSOR_IN_LIQUID);
OneWire owAmbient(PIN_TEMPERATURE_SENSOR_AMBIENT);

DallasTemperature dsBottom(&owBottom);
DallasTemperature dsInLiquid(&owInLiquid);
DallasTemperature dsAmbient(&owAmbient);
DeviceAddress deviceAddresBottom;
DeviceAddress deviceAddressInLiquid;
DeviceAddress deviceAddressAmbient;

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address


float temperatureBottomPlate = 0.0;
float temperatureInLiquid = 0.0;
float temperatureAmbient = 0.0;


String temperatureLog = "";



void temperatureSetup()
{
  Serial.println("The temperature has been initialized.");
  Serial.print("The value of the temperature in the liquid has been set to: [C] ");
  Serial.println(HEATING_TEMPERATURE_LIMIT);

  dsBottom.begin();
  if (!dsBottom.getAddress(deviceAddresBottom, 0)) {
    BIOREACTOR_MODE=BIOREACTOR_ERROR_MODE;
    Serial.println("ALERT: Can not determine temperature of bottom plate");
  } 
  else {
    dsBottom.setWaitForConversion(false);
    dsBottom.setResolution(deviceAddresBottom, 12);
    dsBottom.requestTemperatures();
  }

  dsInLiquid.begin();
  if (!dsInLiquid.getAddress(deviceAddressInLiquid, 0)) {
    BIOREACTOR_MODE=BIOREACTOR_ERROR_MODE;
    Serial.println("ALERT: Can not determine temperature of liquid");
  } 
  else {
    dsInLiquid.setWaitForConversion(false);
    dsInLiquid.setResolution(deviceAddressInLiquid, 12);
    dsInLiquid.requestTemperatures();
  }

  dsAmbient.begin();
  if (!dsAmbient.getAddress(deviceAddressAmbient, 0)) {
    Serial.println("WARNING: Can not determine ambient temperature");
  } 
  else {
    dsAmbient.setWaitForConversion(false);
    dsAmbient.setResolution(deviceAddressAmbient, 12);
    dsAmbient.requestTemperatures();
  }

  //  //write Header for LOG String destined for the .csv file
  //  temperatureLog = "Milisec,Temperature_bottomPlate,Temperature_inLiquid\n";
  //  temperatureLog += "Version:_in_[ms]"; // TODO: use better version control
  //  temperatureLog += millis();
  //  temperatureLog += "\n";
  //
  //  //create directories on the SD file
  //
  //  if(globalSelectSDCard())
  //  {
  //    if(!SD.exists("/LOG"))
  //    {
  //      // create directories if not present
  //      SD.mkdir("/LOG");
  //      if(DEBUG) Serial.println("The /LOG directory has been created.");
  //    }  
  //    else if(DEBUG) Serial.println("The /LOG directory already exists.");
  //  }

  // We want to be sure that the conversion has been done
  delay(600);


}

void temperatureUpdate()
{
  dsBottom.requestTemperatures();
  temperatureBottomPlate=dsBottom.getTempC(deviceAddresBottom);
  if(DEBUG)
  {
    Serial.print("The measured temperature of the Heating-plate is: "); 
    Serial.println(temperatureBottomPlate);
  }
  
  dsInLiquid.requestTemperatures();
  temperatureInLiquid=dsInLiquid.getTempC(deviceAddressInLiquid);
  if(DEBUG)
  {
    Serial.print("The measured temperature of the liquid is: "); 
    Serial.println(temperatureInLiquid);
  }
  
  dsAmbient.requestTemperatures();
  temperatureAmbient=dsAmbient.getTempC(deviceAddressAmbient);
  if(DEBUG)
  {
    Serial.print("The measured ambient temperature is: "); 
    Serial.println(temperatureAmbient);
  }
}


float temperatureMeasuredInLiquid()
{
  return temperatureInLiquid;
}

float temperatureMeasuredBottomPlate()
{
  return temperatureBottomPlate;
}

float temperatureMeasuredAmbient()
{
  return temperatureAmbient;
}


//void temperatureWriteLogToSDcard()
//{     
//  File dataFile;
//  unsigned long writeDuration;
//
//  //make sure that the SD card is selected and the SD card has been initialized
//  if(globalSelectSDCard())
//  {
//    // always include the forward slash if data is in root folder
//    // if the file is not present, it will be created, else the data is being appended
//    dataFile = SD.open("/LOG/reg.log", FILE_WRITE); // pay attention to not use long names!
//
//    // if the file is available, write to it:
//    if (dataFile) 
//    {
//      if(DEBUG) Serial.println("Writing to SD card...");
//      unsigned long stringSize = temperatureLog.length();
//      char buf[stringSize];
//      writeDuration = millis();
//      temperatureLog.toCharArray(buf,sizeof(buf));
//      dataFile.write(buf); // or string
//      dataFile.write("\n"); // workaround because the last end-of-line char of buf is not written correctly
//      dataFile.close();
//      writeDuration = millis()-writeDuration;
//      // print to the serial port too:
//      if(DEBUG) 
//      {    
//        Serial.print("The writing process took [ms]:");      
//        Serial.println(writeDuration);
//      }
//
//      Serial.println("The Data is written on the SD card.");
//
//      //reinitialize string
//      temperatureLog = "";
//    }
//    else // if the file isn't open, pop up an error
//    {
//      Serial.println("ERROR: couldn't open datafile on SD card for writing!");      
//      //try to reinitialize
//      digitalWrite(PIN_SD_CARD, LOW);
//      digitalWrite(PIN_ETHERNET, LOW);
//
//    }
//  }
//  else // if the SD card couldn't be initialized, pop up an error
//  {
//    Serial.println("ERROR: couldn't write to SD card. Data NOT saved!");
//    //try to reinitialize
//    digitalWrite(PIN_SD_CARD, LOW);
//    digitalWrite(PIN_ETHERNET, LOW);
//  }
//
//}









