void loggingSetup();
void loggingEvent();
String loggingURLString();

String logDataString;

void loggingSetup()
{
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
  

  // for the values that have not yet been implemented, "-1" is printed
  char buffer[5];
   //Log all variable data from all sensors 
   //print out the following: TimeStamp[sec],ReactorID[ID],TempSet[C],TempInLiquid[C],TempBottomPlate[C],TempAmbient[C],HeatingPIDregOutput[0;5000],
   logDataString = String(ethernetGetTimeStamp())+",";
   logDataString += String(BIOREACTOR_ID)+",";
   globalFloatToStringDouble(HEATING_TEMPERATURE_LIMIT,2,buffer);
   logDataString += String(buffer)+","; //TempSet
   globalFloatToStringDouble(temperatureMeasuredInLiquid(),2,buffer);
   logDataString += String(buffer)+","; //TempInLiquid
   globalFloatToStringDouble(temperatureMeasuredBottomPlate(),2,buffer);
   logDataString += String(buffer)+","; //TempBottomPlate
   globalFloatToStringDouble(temperatureMeasuredAmbient(),2,buffer);
   logDataString += String(buffer)+","; //TempAmbient
   logDataString += String(heatingPIDRegOutput())+",";
   // print out the following LiquidLevel[inch],LiquidIN[secON/60sec],LiquidOUT[secON/60sec],pH[1;14],AcidIN[secON/60sec],BaseIn[secON/60sec],
   globalFloatToStringDouble(liquidLevelGet(),2,buffer);
   logDataString += String(buffer)+","; //Liquid level hight //for debugging in inch
   logDataString += String(-1)+","; //LiquidIN[secON/60sec]
   logDataString += String(-1)+","; //LiquidOUT[secON/60sec]
   globalFloatToStringDouble(pHmeterGet(),2,buffer);
   logDataString += String(buffer)+","; //pH level
   logDataString += String(-1)+","; //AcidIN[secON/60sec]
   logDataString += String(-1)+","; // BaseIn[secON/60sec]
   logDataString += String(relaySwitchMotorGetState())+",";
    // print out the following MotorState[OFF;ON],NH4[concentr],GazCH4IN[secON/60sec],GazCO2IN[secON/60sec],BioreactorState[0:2]
   logDataString += String(-1)+","; //NH4[concentr]
   logDataString += String(-1)+","; // GazCH4IN[secON/60sec]
   logDataString += String(-1)+","; //GazCO2IN[secON/60sec]
   logDataString += String(-1)+",";  //GazN2IN[secON/60sec]
   logDataString += String(BIOREACTOR_MODE);
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
