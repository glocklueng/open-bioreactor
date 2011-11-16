void loggingSetup();
void loggingEvent();

char logDataString[LOG_MAX_LENGTH];

void loggingSetup()
{
  Serial.println("Logging successfully initialized.");
}

void loggingEvent()
{
  int notImplemented = -1; // for the values that have not yet been implemented, "-1" is printed
  int logDataStringLength;

  // if (DEBUG) Serial.println("logging...");

  /*
  Log all variable data from all sensors 
   print out the following: TimeStamp[sec],ReactorID[ID],TempSet[C],TempInLiquid[C],TempBottomPlate[C],TempAmbient[C],
   HeatingPIDregOutut[0; 5000],LiquidLevel[inch],LiquidIN[secON/60sec],LiquidOUT[secON/60sec],pH[1;14],AcidIN[secON/60sec],
   BaseIn[secON/60sec],MotorState[OFF;ON],NH4[concentr],GazCH4IN[secON/60sec],GazCO2IN[secON/60sec],GazN2IN[secON/60sec]BioreactorState[0:2]
   */



  logDataStringLength = sprintf(logDataString, 
  "%lu,%d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d,%d,%d,%d.%02d,%d,%d,%d,%d,%d,%d,%d,%d",
  ethernetGetTimeStamp(),
  BIOREACTOR_ID,
  (int)HEATING_TEMPERATURE_LIMIT, (int)(((HEATING_TEMPERATURE_LIMIT - (int)HEATING_TEMPERATURE_LIMIT)*100)),
  (int)temperatureMeasuredInLiquid(), (int)(((temperatureMeasuredInLiquid() - (int)temperatureMeasuredInLiquid())*100)),
  (int)temperatureMeasuredBottomPlate(), (int)(((temperatureMeasuredBottomPlate() - (int)temperatureMeasuredBottomPlate())*100)),
  (int)temperatureMeasuredAmbient(), (int)(((temperatureMeasuredAmbient() - (int)temperatureMeasuredAmbient())*100)),
  (int)heatingPIDRegOutput(), (int)(((heatingPIDRegOutput() - (int)heatingPIDRegOutput())*100)),
  (int)liquidLevelGet(),
  notImplemented,
  notImplemented,
  (int)pHmeterGet(), (int)(((pHmeterGet() - (int)pHmeterGet())*100)),
  notImplemented,
  notImplemented,
  relaySwitchMotorGetState(),
  gasValvesGetAutoSwitchInterval(CH4),
  gasValvesGetAutoSwitchInterval(CO2),
  gasValvesGetAutoSwitchInterval(N2),
  notImplemented,
  BIOREACTOR_MODE
    );

  //no line end! Threat this separately for logging into the SD card and via ethernet on the server


  if(DEBUG && false)
  {
    Serial.println(logDataString);
    Serial.print("Size of the string is: ");
    Serial.println(logDataStringLength);
  }

  ethernetPushLog(logDataString); 

}







