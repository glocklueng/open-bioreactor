//
//void temperatureSetup();
//void temperatureUpdate();
//boolean temperatureGetOneWire(byte *addr, float *retnTemp, OneWire * OW);
////void temperatureWriteLogToSDcard();
//float temperatureMeasuredInLiquid();
//float temperatureMeasuredBottomPlate();
//float temperatureMeasuredAmbient();
//
//
///*
//This code was taken from the Arduino forum (arduino.cc).
//It has been modified to meet the project's needs.
//*/
//
//
//// DS18S20 Temperature chip i/o
//OneWire dsBottom(PIN_TEMPERATURE_SENSOR_BOTTOM);
//OneWire dsInLiquid(PIN_TEMPERATURE_SENSOR_IN_LIQUID);
//OneWire dsAmbient(PIN_TEMPERATURE_SENSOR_AMBIENT);
////ID of temperature sensor on the bottom of the heating plate
//byte SENSOR_BOTTOM[8]= {
//  0x28,0x3F,0xA8,0x23,0x03,0x00,0x00,0xC6};
//byte SENSOR_IN_LIQUID[8] = {
//  0x28,0xD0,0x93,0x64,0x03,0x00,0x00,0x59};
//byte SENSOR_AMBIENT[8] = {
//  0x28,0xD4,0xA5,0x23,0x03,0x00,0x00,0xFB};
//
//
//float temperatureBottomPlate = 0.0;
//float temperatureInLiquid = 0.0;
//float temperatureAmbient = 0.0;
//
//
//String temperatureLog = "";
//
//
//
//void temperatureSetup()
//{
//  Serial.println("The temperature has been initialized.");
//  Serial.print("The value of the temperature in the liquid has been set to: [C] ");
//  Serial.println(HEATING_TEMPERATURE_LIMIT);
//  
////  //write Header for LOG String destined for the .csv file
////  temperatureLog = "Milisec,Temperature_bottomPlate,Temperature_inLiquid\n";
////  temperatureLog += "Version:_in_[ms]"; // TODO: use better version control
////  temperatureLog += millis();
////  temperatureLog += "\n";
////
////  //create directories on the SD file
////
////  if(globalSelectSDCard())
////  {
////    if(!SD.exists("/LOG"))
////    {
////      // create directories if not present
////      SD.mkdir("/LOG");
////      if(DEBUG) Serial.println("The /LOG directory has been created.");
////    }  
////    else if(DEBUG) Serial.println("The /LOG directory already exists.");
////  }
//
//
//}
//
//void temperatureUpdate()
//{
//  unsigned long exactPresentTime;
//  char charBufferTemperatureBottomPlate[8];
//  char charBufferTemperatureInLiquid[8];
//  char charBufferTemperatureAmbient[8];
//  
//  //get temperature on the bottom plate
//  if (temperatureGetOneWire(SENSOR_BOTTOM,&temperatureBottomPlate,&dsBottom))
//  {
//    if(DEBUG)
//    {
//      Serial.print("The measured temperature of the Heating-plate is: "); 
//      globalFloatToStringDouble(temperatureBottomPlate,2,charBufferTemperatureBottomPlate); 
//      Serial.println(charBufferTemperatureBottomPlate);
//    }
//  }
//  else Serial.println("ERROR: Temperature on heating resistance could NOT have been measured correctly!");
//
//  //get temperature in the liquid
//  if (temperatureGetOneWire(SENSOR_IN_LIQUID,&temperatureInLiquid,&dsInLiquid))
//  {
//    if(DEBUG)
//    {
//      Serial.print("The measured temperature in the liquid is: ");  
//      globalFloatToStringDouble(temperatureInLiquid,2,charBufferTemperatureInLiquid);
//      Serial.println(charBufferTemperatureInLiquid);
//    }
//  }
//  else Serial.println("ERROR: Temperature in liquid could NOT have been measured correctly!");
//  
//    //get ambient temperature 
//  if (temperatureGetOneWire(SENSOR_AMBIENT,&temperatureAmbient,&dsAmbient))
//  {
//    if(DEBUG)
//    {
//      Serial.print("The measured ambient temperature is: ");  
//      globalFloatToStringDouble(temperatureAmbient,2,charBufferTemperatureAmbient);
//      Serial.println(charBufferTemperatureAmbient);
//    }
//  }
//  else Serial.println("ERROR: Ambient temperature could NOT have been measured correctly!");
////
////  //Save the values in the LOG String
////  exactPresentTime = millis();
////  temperatureLog += exactPresentTime;
////  temperatureLog += ",";
////  temperatureLog += charBufferTemperatureBottomPlate;
////  temperatureLog += ",";
////  temperatureLog += charBufferTemperatureInLiquid;
////  temperatureLog += "\n";
////
////  //write to SD card if the String buffer has reached max. defined length
////  if(temperatureLog.length() >= LOG_TEMPERATURE_LENGTH) // write to SD card.
////  {
////    if(DEBUG)
////    {
////      Serial.println("The String buffer is going to be written on the SD card.");
////      Serial.print("The size of the Temperature data String is: ");
////      Serial.println(temperatureLog.length());  
////      //Serial.print("The dataString is: \n\n");
////      //Serial.println(temperatureLog);
////    }
////    temperatureWriteLogToSDcard();
////    
////  }
//
//}
//
//
//
//boolean temperatureGetOneWire(byte *addr, float *retnTemp, OneWire * OW)
//{
//  //Input: Pointer to the OneWire address of the temperature sensor; 
//  //       Pointer to a place to write the sensor reading; 
//  //       Pointer to the OneWire object
//
//  byte present = 0;
//  byte data[12];
//  int raw;
//
//  OW->reset();
//  OW->select(addr);
//  OW->write(0x44,0); // start conversion, with parasite power OFF at the end
//
//  // NO delay
//  //delay(800);     // maybe 750ms is enough, maybe not
//  // we might do a ds.depower() here, but the reset will take care of it.
//
//  present = OW->reset();
//  if (present)  //only continue if the sensor is actually there and responding
//  {
//    OW->select(addr);    
//    OW->write(0xBE);         // Read Scratchpad which now contains the temperature data
//    for ( int i = 0; i < 9; i++) {           // we need 9 bytes
//      data[i] = OW->read();
//    }
//    if ( OneWire::crc8( data, 8) != data[8]) {
//      if(DEBUG) Serial.println("CRC is not valid!\n");
//      return false;
//    }
//    raw=(data[1]<<8)+data[0];  //put the two bytes of the temperature (from the response) into a raw int
//    *retnTemp = (float)raw * 0.0625; //convert to celcius
//    return true;
//  }
//  return false;
//}
//
//
//
////void temperatureWriteLogToSDcard()
////{     
////  File dataFile;
////  unsigned long writeDuration;
////
////  //make sure that the SD card is selected and the SD card has been initialized
////  if(globalSelectSDCard())
////  {
////    // always include the forward slash if data is in root folder
////    // if the file is not present, it will be created, else the data is being appended
////    dataFile = SD.open("/LOG/reg.log", FILE_WRITE); // pay attention to not use long names!
////
////    // if the file is available, write to it:
////    if (dataFile) 
////    {
////      if(DEBUG) Serial.println("Writing to SD card...");
////      unsigned long stringSize = temperatureLog.length();
////      char buf[stringSize];
////      writeDuration = millis();
////      temperatureLog.toCharArray(buf,sizeof(buf));
////      dataFile.write(buf); // or string
////      dataFile.write("\n"); // workaround because the last end-of-line char of buf is not written correctly
////      dataFile.close();
////      writeDuration = millis()-writeDuration;
////      // print to the serial port too:
////      if(DEBUG) 
////      {    
////        Serial.print("The writing process took [ms]:");      
////        Serial.println(writeDuration);
////      }
////
////      Serial.println("The Data is written on the SD card.");
////
////      //reinitialize string
////      temperatureLog = "";
////    }
////    else // if the file isn't open, pop up an error
////    {
////      Serial.println("ERROR: couldn't open datafile on SD card for writing!");      
////      //try to reinitialize
////      digitalWrite(PIN_SD_CARD, LOW);
////      digitalWrite(PIN_ETHERNET, LOW);
////
////    }
////  }
////  else // if the SD card couldn't be initialized, pop up an error
////  {
////    Serial.println("ERROR: couldn't write to SD card. Data NOT saved!");
////    //try to reinitialize
////    digitalWrite(PIN_SD_CARD, LOW);
////    digitalWrite(PIN_ETHERNET, LOW);
////  }
////
////}
//
//float temperatureMeasuredInLiquid()
//{
//  return temperatureInLiquid;
//}
//
//float temperatureMeasuredBottomPlate()
//{
//  return temperatureBottomPlate;
//}
//
//float temperatureMeasuredAmbient()
//{
//  return temperatureAmbient;
//}


float temperatureMeasuredInLiquid()
{
  return 20;
}

float temperatureMeasuredBottomPlate()
{
  return 20;
}

float temperatureMeasuredAmbient()
{
  return 20;
}





