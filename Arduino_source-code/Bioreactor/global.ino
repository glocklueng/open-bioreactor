void globalFloatToStringDouble(double val, byte precision, char *buf, unsigned bufLen = 0xffff);
void globalGetCommandAndPushLog();
//boolean globalSelectSDCard();
//boolean globalSelectEthernet();


void globalUpdateSensors()
{
  temperatureUpdate();
  pHmeterUpdate();
  liquidLevelUpdate();
  floatUpdate();  
}

void globalGetCommandAndPushLog()
{
  //the network related functions have to be together; 
  //else there is a memory allocation failure of the "String"s (bug in Arduino IDE 0022)
  ethernetGetCommand();
  loggingEvent();  
}



//boolean globalSelectSDCard()
//{
//  if(arduinoETHShieldMode != MODE_SDCARD)
//  {
//  //disable the Ethernet pin and (re-)initialize the card.
//    if(DEBUG) Serial.print("\n\nInitializing SD card...");
//    // see if the card is present and can be initialized:
//    digitalWrite(PIN_ETHERNET, HIGH); // deselect Ethernet (MUST be HIGH); PIN_SD_CARD can have any value
//    if (!SD.begin(PIN_SD_CARD)) 
//    {
//      if(DEBUG) Serial.println("ERROR: SD Card failed to initialize, or not present!");
//      return 0;
//    }
//    else 
//    {
//      if(DEBUG )Serial.println("SD card initialized correctly.");
//      arduinoETHShieldMode = MODE_SDCARD;
//      return 1;
//    }
//  }
//  else if(DEBUG)Serial.println("SD card already initialized!");
//
//}


//boolean globalSelectEthernet()
//{
//  if(arduinoETHShieldMode != MODE_ETHERNET)
//  {
//  //disable the SD card pin and (re-)initialize
//    if(DEBUG) Serial.print("\n\nInitializing Ethernet...");
//    digitalWrite(PIN_SD_CARD, HIGH); // deselect SD card (MUST be HIGH);
//    if(DEBUG )Serial.println("Ethernet initialized correctly.");
//    arduinoETHShieldMode = MODE_ETHERNET;
//    return 1;
//    
//  }
//  else if(DEBUG)Serial.println("Ethernet already initialized!");
//
//}



