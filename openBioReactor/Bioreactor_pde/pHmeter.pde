void pHmeterUpdate();
float pHmeterGet();

//----------CONSTANTS---------


float pHLevelGlobal;


void pHmeterUpdate()
{
  int pHAnalogValue;
  float pHLevel;
  
  //the pH level output of the pH meter is between 0 and 4 Volts
  //the AD converter of the Ardnuino converts the analog signal 
  //between 0 and 5 Volts into 1024 intervals
  pHAnalogValue = analogRead(PIN_PH_METER);
  
  pHLevel = (float)14 * pHAnalogValue / pH14;
  pHLevelGlobal = pHLevel; // update global variable
  
  if(pHLevel >14)
  {
   Serial.println("WARNING: The pH level is higher than 14! The pH sonde might not be attached correctly!"); 
  }
  
  if(DEBUG)
  {
    Serial.print("The pH value of the liquid is: ");
    Serial.println(pHLevel);
    Serial.print("pH of the liquid in AD-intervals (Arduino): ");
    Serial.println(pHAnalogValue);  
    Serial.print("pH in MiliVolts: ");
    Serial.println(pHAnalogValue*4.88); // 4.88= 5[Volts of input]/1024 [AD-intervals]
  }
  
}

float pHmeterGet()
{
return pHLevelGlobal;
}
