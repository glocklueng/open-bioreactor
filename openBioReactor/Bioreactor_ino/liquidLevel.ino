void liquidLevelSetup();
void liquidLevelUpdate();
float liquidLevelGet();


//int liquidLevelFull = 440; //sometimes 363;  // when the reactor is full
//int liquidLevelEmpty = 628; //sometimes 716; // when the reactor is empty (no liquid up to 1 inch of liquid)
//                            // Hight Vout values if reactor is empty
//                            // see eTape (PN-6573P-8) datasheet for more information
//                            // originally calibrated values: 353 if full and 714 if empty 
//float liquidSensorLength = 21.84; // [cm] according to datasheet
//float reservoirDiameter = 20.5; // [cm] !!only approx value

float liquidHeightGlobal; // gloabl variable for export; read value of the analog Pin [0;1023]
                          // the exact height and the volume are calculated on the server


void liquidLevelSetup()
{
  
  
}


void liquidLevelUpdate() 
{
  int liquidLevelAtPin;
  
  liquidLevelAtPin = analogRead(PIN_LIQUID_LEVEL);
  liquidHeightGlobal = liquidLevelAtPin; // the lookup table is on the server; TODO: clean up
  

  // the conversation is done on the server for the exact liquid level and the liquid volume
  // only the read analog value of the pin is uploaded to the server.
  

//  //check in which interval the read anolog value is and display the according height value [inch]
//  //max value from AD-converter: 1023
//
//  float liquidPercentFull, liquidHeight, liquidVolume, errorLiquidbySensor;
//  float liquidSlopeCalibration = -0.0379;
//  float liquidOffsetCalibration = 24.387;
//
//
//  if(liquidLevelAtPin<=1023 && liquidLevelAtPin >= 1020)
//  {
//    liquidHeightGlobal = 1.5;
//  }
//  else if(liquidLevelAtPin<1020 && liquidLevelAtPin >= 1011)
//  {
//    liquidHeightGlobal = 1.75;
//  }
//  else if(liquidLevelAtPin<1011 && liquidLevelAtPin >= 1007)
//  {
//    liquidHeightGlobal = 2.0;
//  }
//  else if(liquidLevelAtPin<1007 && liquidLevelAtPin >= 942)
//  {
//    liquidHeightGlobal = 2.25;
//  }  
//  else if(liquidLevelAtPin<942 && liquidLevelAtPin >= 923)
//  {
//    liquidHeightGlobal = 2.5;
//  }
//  else if(liquidLevelAtPin<923 && liquidLevelAtPin >= 906)
//  {
//    liquidHeightGlobal = 2.75;
//  }
//  else if(liquidLevelAtPin<906 && liquidLevelAtPin >= 898)
//  {
//    liquidHeightGlobal = 3.0;
//  } 
//  else if(liquidLevelAtPin<898 && liquidLevelAtPin >= 877)
//  {
//    liquidHeightGlobal = 3.25;
//  }
//  else if(liquidLevelAtPin<877 && liquidLevelAtPin >= 844)
//  {
//    liquidHeightGlobal = 3.5;
//  }
//  else if(liquidLevelAtPin<844 && liquidLevelAtPin >= 797)
//  {
//    liquidHeightGlobal = 3.75;
//  }  
//  else if(liquidLevelAtPin<797 && liquidLevelAtPin >= 786)
//  {
//    liquidHeightGlobal = 4.0;
//  }
//  else if(liquidLevelAtPin<786 && liquidLevelAtPin >= 764)
//  {
//    liquidHeightGlobal = 4.25;
//  }
//  else if(liquidLevelAtPin<764 && liquidLevelAtPin >= 723)
//  {
//    liquidHeightGlobal = 4.5;
//  }
//  else if(liquidLevelAtPin<723 && liquidLevelAtPin >= 715)
//  {
//    liquidHeightGlobal = 4.75;
//  }
//  else if(liquidLevelAtPin<715 && liquidLevelAtPin >= 697)
//  {
//    liquidHeightGlobal = 5.0;
//  }
//    else if(liquidLevelAtPin<697 && liquidLevelAtPin >= 673)
//  {
//    liquidHeightGlobal = 5.25;
//  }  
//  else if(liquidLevelAtPin<673 && liquidLevelAtPin >= 647)
//  {
//    liquidHeightGlobal = 5.5;
//  }
//  else if(liquidLevelAtPin<647 && liquidLevelAtPin >= 620)
//  {
//    liquidHeightGlobal = 5.75;
//  }
//  else if(liquidLevelAtPin<620 && liquidLevelAtPin >= 613)
//  {
//    liquidHeightGlobal = 6.0;
//  } 
//  else if(liquidLevelAtPin<613 && liquidLevelAtPin >= 610)
//  {
//    liquidHeightGlobal = 6.25;
//  }
//  else if(liquidLevelAtPin<610 && liquidLevelAtPin >= 600)
//  {
//    liquidHeightGlobal = 6.5;
//  }
//  else if(liquidLevelAtPin<600 && liquidLevelAtPin >= 572)
//  {
//    liquidHeightGlobal = 6.75;
//  }  
//  else if(liquidLevelAtPin<572 && liquidLevelAtPin >= 542)
//  {
//    liquidHeightGlobal = 7.0;
//  }
//  else if(liquidLevelAtPin<542 && liquidLevelAtPin >= 526)
//  {
//    liquidHeightGlobal = 7.25;
//  }
//  else if(liquidLevelAtPin<526 && liquidLevelAtPin >= 524)
//  {
//    liquidHeightGlobal = 7.5;
//  }
//  else if(liquidLevelAtPin<524 && liquidLevelAtPin >= 521)
//  {
//    liquidHeightGlobal = 7.75;
//  }
//  else if(liquidLevelAtPin<521 && liquidLevelAtPin >= 514)
//  {
//    liquidHeightGlobal = 8.0;
//  }
//    else if(liquidLevelAtPin<514 && liquidLevelAtPin >= 505)
//  {
//    liquidHeightGlobal = 8.25;
//  }
//  else if(liquidLevelAtPin<505 && liquidLevelAtPin >= 503)
//  {
//    liquidHeightGlobal = 8.5;
//  }
//  else if(liquidLevelAtPin<503)
//  {
//    liquidHeightGlobal = 8.6; //MAX
//  }   
//
//
// 
// 
//  //if outside the range, adjust it for being in the range
//  if(liquidLevelAtPin > liquidLevelEmpty) // pay attention to the sign/sense
//  {
//    liquidLevelAtPin=liquidLevelEmpty;
//    if(DEBUG)Serial.println("ERROR: The measured liquid is too high. It has been adjustet to fit the maximum value.");
//  }
//  else if(liquidLevelAtPin < liquidLevelFull) 
//  {
//    liquidLevelAtPin=liquidLevelFull;
//    if(DEBUG)Serial.println("ERROR: The measured liquid is too low. It has been adjustet to fit the minimum value.");
//  }
//  
//  //liquidHeightGlobal = (float)liquidSlopeCalibration*liquidLevelAtPin + liquidOffsetCalibration;
//  
//  //liquidPercentFull = (float)(liquidLevelEmpty-liquidLevelAtPin)/(liquidLevelEmpty-liquidLevelFull); 
//  //liquidHeight = liquidPercentFull*liquidSensorLength;
//  //liquidVolume = PI*(reservoirDiameter/2)*(reservoirDiameter/2)*liquidHeightGlobal*2.54; //cm^3=mL; ONLY APPROX: because the cylinder in not perfectly round
//  //liquidVolume = liquidPercentFull*7.35; // max capacity is 7.35 liters (until the Max value of the sensor)
//  
//  //if(liquidPercentFull>1.0)Serial.println("ERROR: more than 100% liquid!");
//  //Approximate error: 10% on eTape resistance value by sensor (see datasheet)
//  //errorLiquidbySensor = 0.1*...;
//  
//  //update the char string with liquid level
//  liquidHeightGlobal = liquidHeight; // in [cm]
//  liquidHeightGlobal *= 0.394; // in [inch] 
//  
//  if(DEBUG)
//  {
//    Serial.print("Liquid level in Volt-interval on Arduino: ");
//    Serial.println(liquidLevelAtPin);       
//    Serial.print("Liquid level in cm: ");
//    Serial.println(liquidHeight);
//    Serial.print("Liquid level in inch: ");
//    Serial.println(liquidHeightGlobal);
//    Serial.print("Liquid level in liters: ");
//    Serial.println(liquidHeightVolume);
//    Serial.print("Liquid level in percent: ");
//    Serial.println(liquidPercentFull); 
//  }
//  
//
//  float liquidHeightInterval = liquidLevelAtPin;
//  float liquidHeightVolume = 0.85*liquidHeightGlobal;

}

float liquidLevelGet()
{
  return liquidHeightGlobal;
}

