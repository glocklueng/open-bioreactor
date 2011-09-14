void loggingSetup();
void loggingEvent();
String loggingURLString();

String logDataString;

//char logDataString;

void loggingSetup()
{
//
//     
//  //Write header to
//       if(DEBUG)
//   {
//     Serial.println(logDataString);
//     Serial.print("Size of the string is: ");
//     Serial.println(logDataString.length());
//   }
//  logDataString = "TimeStamp[sec],ReactorID[ID],TempSet[C],TempInLiquid[C],TempBottomPlate[C],TempAmbient[C],HeatingPIDregOutput[0;5000],LiquidLevel[inch],LiquidIN[secON/60sec],LiquidOUT[secON/60sec],pH[1;14],AcidIN[secON/60sec],AcidOUT[secON/60sec],MotorState[OFF;ON],NH4[concentr],GazCH4IN[secON/60sec],GazCO2IN[secON/60sec],BioreactorState[0:2]";
//     if(DEBUG)
//   {
//     Serial.println(logDataString);
//     Serial.print("Size of the string is: ");
//     Serial.println(logDataString.length());
//   }
   Serial.println("Logging successfully initialized.");
}

void loggingEvent()
{
  if(DEBUG)Serial.println("logging...");
  
  if(logDataString.length() > LOG_MAX_LENGTH)
  {
     Serial.println("WARNING: The log string exceeded the maximum log length! It has been reinitialized.");
     logDataString = "";
  }
  
  // first update the NTP time
  ethernetUpdateNTPTime();
  if(DEBUG)Serial.println("Time stamp updated.");
  
  // for the values that have not yet been implemented, "-1" is printed
  char buffer[5];
   //Log all variable data from all sensors 
   //print out the following: TimeStamp[sec],ReactorID[ID],TempSet[C],TempInLiquid[C],TempBottomPlate[C],TempAmbient[C],HeatingPIDregOutput[0;5000],
   logDataString = ethernetGetTimeStamp();
   logDataString += ",";
   logDataString += BIOREACTOR_ID;
   logDataString += ",";
   globalFloatToStringDouble(HEATING_TEMPERATURE_LIMIT,2,buffer);
   logDataString += buffer; //TempSet
   logDataString += ",";
   globalFloatToStringDouble(temperatureMeasuredInLiquid(),2,buffer);
   logDataString += buffer; //TempInLiquid
   logDataString += ",";
   globalFloatToStringDouble(temperatureMeasuredBottomPlate(),2,buffer);
   logDataString += buffer; //TempBottomPlate
   logDataString += ",";
   globalFloatToStringDouble(temperatureMeasuredAmbient(),2,buffer);
   logDataString += buffer; //TempAmbient
   logDataString += ",";   
   logDataString += heatingPIDRegOutput();
   logDataString += ",";
   // print out the following LiquidLevel[inch],LiquidIN[secON/60sec],LiquidOUT[secON/60sec],pH[1;14],AcidIN[secON/60sec],AcidOUT[secON/60sec],
   globalFloatToStringDouble(liquidLevelGet(),2,buffer);
   logDataString += buffer; //Liquid level hight //for debugging in inch
   logDataString += ",";
   globalFloatToStringDouble(liquidLevelGetInverval(),1,buffer);
   logDataString += buffer;
//   logDataString += -1; //LiquidIN[secON/60sec]
   logDataString += ",";
   globalFloatToStringDouble(liquidLevelGetVolume(),2,buffer);
   logDataString += buffer;
//   logDataString += -1; //LiquidOUT[secON/60sec]
   logDataString += ",";
   globalFloatToStringDouble(pHmeterGet(),2,buffer);
   logDataString += buffer; //pH level
   logDataString += ",";
   logDataString += -1; //AcidIN[secON/60sec]
   logDataString += ",";   
   logDataString += -1; // AcidOUT[secON/60sec]
   logDataString += ",";
   logDataString += relaySwitchMotorGetState();
   logDataString += ",";
    // print out the following MotorState[OFF;ON],NH4[concentr],GazCH4IN[secON/60sec],GazCO2IN[secON/60sec],BioreactorState[0:2]\n
   logDataString += -1; //NH4[concentr]
   logDataString += ",";   
   logDataString += -1; // GazCH4IN[secON/60sec]
   logDataString += ",";
   logDataString += -1; //GazCO2IN[secON/60sec]
   logDataString += ",";
   logDataString += BIOREACTOR_MODE;
   //no line end! Threat this separately for logging into the SD card and via ethernet on the server
   
   
   if(DEBUG)
   {
     Serial.println(logDataString);
     Serial.print("Size of the string is: ");
     Serial.println(logDataString.length());
   }

   ethernetPushLog(logDataString); 

}

String loggingURLString()
{
  return logDataString;  
}
