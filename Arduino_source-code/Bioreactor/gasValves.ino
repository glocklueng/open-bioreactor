void gasValvesSetup();
void gasValvesTurnOff(int gasValveID);
void gasValvesTurnOn(int gasValveID);
int gasValvesGetState(int gasValveID);
int gasValvesGetAutoSwitchInterval(int gasValveID);
void gasValvesSetAutoSwitchInterval(int gasValveID, int percent);
void gasValvesAutoSwitchMode();
void gasValvesCheck();

struct gasValveStruct { int state;
                        int autoSwitchInterval; // in [%] percent [0:100]
                        boolean autoSwitchFlag;
                        int autoSwitchState;
                        int arduinoPin;  
                        unsigned long timestampOn;
                        unsigned long timestampOff;
                       };

struct gasValveStruct gasValve[3];


void gasValvesSetup()
{
   pinMode(PIN_GAS_CH4, OUTPUT);
   pinMode(PIN_GAS_CO2, OUTPUT);
   pinMode(PIN_GAS_N2, OUTPUT);
   
   gasValve[CH4].state = OFF;
   gasValve[CH4].autoSwitchInterval = 0; // percent
   gasValve[CH4].autoSwitchFlag = false;
   gasValve[CH4].arduinoPin = PIN_GAS_CH4;
   gasValve[CO2].state = OFF;
   gasValve[CO2].autoSwitchInterval = 0;
   gasValve[CO2].autoSwitchFlag = false;
   gasValve[CO2].arduinoPin = PIN_GAS_CO2;
   gasValve[N2].state = OFF;
   gasValve[N2].autoSwitchInterval = 0;
   gasValve[N2].autoSwitchFlag = false;
   gasValve[N2].arduinoPin = PIN_GAS_N2;
   
}


void gasValvesTurnOff(int gasValveID)
{
   switch (gasValveID)
   {
     case CH4:
       digitalWrite(gasValve[gasValveID].arduinoPin, LOW);
       gasValve[gasValveID].state = OFF;
       if(DEBUG)Serial.println("Gas valve for CH4 has been turned off.");
       break;
     case CO2:
       digitalWrite(gasValve[gasValveID].arduinoPin, LOW);
       gasValve[gasValveID].state = OFF;
       if(DEBUG)Serial.println("Gas valve for CO2 has been turned off.");
       break;
     case N2:
       digitalWrite(gasValve[gasValveID].arduinoPin, LOW);
       gasValve[gasValveID].state = OFF;
       if(DEBUG)Serial.println("Gas valve for N2 has been turned off.");
       break;
     case ALL:
       digitalWrite(gasValve[CH4].arduinoPin, LOW);
       gasValve[CH4].state = OFF;
       digitalWrite(gasValve[CO2].arduinoPin, LOW);
       gasValve[CO2].state = OFF;
       digitalWrite(gasValve[N2].arduinoPin, LOW); 
       gasValve[N2].state = OFF;      
       if(DEBUG)Serial.println("All gas valves have been turned off.");
       break;
     
     default:
       Serial.println("ERROR: Invalid gas valve ID! Gas valve cannot be turned OFF");
     
   }    
  
}

void gasValvesTurnOn(int gasValveID)
{
   switch (gasValveID)
   {
     case CH4:
       digitalWrite(gasValve[gasValveID].arduinoPin, HIGH);
       gasValve[gasValveID].state = ON;
       if(DEBUG)Serial.println("Gas valve for CH4 has been turned on.");
       break;
     case CO2:
       digitalWrite(gasValve[gasValveID].arduinoPin, HIGH);
       gasValve[gasValveID].state = ON;
       if(DEBUG)Serial.println("Gas valve for CO2 has been turned on.");
       break;
     case N2:
       digitalWrite(gasValve[gasValveID].arduinoPin, HIGH);
       gasValve[gasValveID].state = ON;
       if(DEBUG)Serial.println("Gas valve for N2 has been turned on.");
       break;
     case ALL:
       digitalWrite(gasValve[CH4].arduinoPin, HIGH);
       gasValve[CH4].state = ON;
       digitalWrite(gasValve[CO2].arduinoPin, HIGH);
       gasValve[CO2].state = ON;
       digitalWrite(gasValve[N2].arduinoPin, HIGH); 
       gasValve[N2].state = ON;      
       if(DEBUG)Serial.println("All gas valves have been turned on.");
       break;
     
     default:
       Serial.println("ERROR: Invalid gas valve ID! Gas valve cannot be turned ON");
     
   }    
  
}


int gasValvesGetState(int gasValveID)
{
  if (gasValveID == CH4
      || gasValveID == CO2
      || gasValveID == N2)
  {
    return gasValve[gasValveID].state;
  }
  else
  {
    Serial.println("ERROR: Invalid gas valve ID! Gas valve's state cannot be returned.");
    return -1;   
  }    
}


int gasValvesGetAutoSwitchInterval(int gasValveID) // GET
{
  if (gasValveID == CH4
      || gasValveID == CO2
      || gasValveID == N2)
  {
    return gasValve[gasValveID].autoSwitchInterval;
  }
  else
  {
    Serial.println("ERROR: Invalid gas valve ID! Gas valve's AutoSwitchInterval cannot be returned.");
    return -1;   
  }    
}




void gasValvesSetAutoSwitchInterval(int gasValveID, int percent) // SET
{
    if (gasValveID == CH4
        || gasValveID == CO2
        || gasValveID == N2)    
    {
      gasValve[gasValveID].autoSwitchInterval = percent;
    }
    else
    {
      Serial.println("ERROR: Invalid gas valve ID! Gas valve's AutoSwitchInterval cannot be saved.");
    }
}

void gasValvesAutoSwitchMode(int gasValveID)
{
    //preliminary checking of the extreme values
    if(gasValve[gasValveID].autoSwitchInterval <= 0)
    {
       gasValve[gasValveID].state = OFF;
       gasValve[gasValveID].autoSwitchInterval = 0; //security: if an invalid number has been saved over webUI
       gasValve[gasValveID].autoSwitchFlag = false;
       return;
    }
    if(gasValve[gasValveID].autoSwitchInterval >= 100)
    {
       gasValve[gasValveID].state = ON;
       gasValve[gasValveID].autoSwitchInterval = 100; //security: if an invalid number has been saved over webUI
       gasValve[gasValveID].autoSwitchFlag = false;
       return;
    }

    
    //check if gas valve is turned on over WebUI
    //if so, put the gas valve in auto-switching mode and start the timing
    if(gasValve[gasValveID].state == ON)
    {
      gasValve[gasValveID].autoSwitchFlag = true;
      gasValve[gasValveID].autoSwitchState = ON;
      gasValve[gasValveID].timestampOn = millis();
      gasValve[gasValveID].timestampOff = millis();
    }
    else
    {
      gasValve[gasValveID].autoSwitchFlag = false; //turn off flag if OFF over webUI
    }
    
    if(gasValve[gasValveID].state == ON && gasValve[gasValveID].autoSwitchFlag == true)
    {
      gasValvesCheck();
      if(DEBUG)Serial.println("Gas valves auto-switching mode is being called successfully."); 
    }
    else
    {
      Serial.println("WARNING: The gas valve is turned off in the WebUI. Please turn on for auto-swithing mode <100% of gas flow");
    }
   
}


// this function is called as well by the main Bioreactor loop
void gasValvesCheck()
{
  unsigned long nowCheck;
  unsigned long offDuration;
  unsigned long onDuration;
  unsigned long timeInterval = 60000; // equals 1 min
  
  /*Note: as defined in main: 
  #define CH4 0
  #define CO2 1
  #define N2 2
  */
  
  for(int i=0; i=2; i++)
  {
    if(gasValve[i].autoSwitchFlag == true)
    {
      onDuration = gasValve[i].autoSwitchInterval * timeInterval;
      offDuration = timeInterval - onDuration;
      nowCheck = millis();
      
      if((gasValve[i].autoSwitchState == ON) && ((nowCheck - gasValve[i].timestampOn) >= onDuration))
      {
        digitalWrite(gasValve[i].arduinoPin, LOW); //switch off
        gasValve[i].autoSwitchState = OFF;
        if(DEBUG)
          {
            Serial.print("The overtime for valve with ID: ");
            Serial.print(i);
            Serial.print(" being ON is [millisec]: ");
            unsigned long timeDiff = gasValve[i].timestampOn - nowCheck;
            Serial.println(timeDiff);
          }
          
        gasValve[i].timestampOff = nowCheck; // NOTE: timestampOFF!
      }//end of onDuration
            
      if((gasValve[i].autoSwitchState == OFF) && ((nowCheck - gasValve[i].timestampOff) >= offDuration))
      {
        digitalWrite(gasValve[i].arduinoPin, HIGH); //switch on
        gasValve[i].autoSwitchState = ON;
        if(DEBUG)
          {
            Serial.print("The overtime for valve with ID: ");
            Serial.print(i);
            Serial.print(" being OFF is [millisec]: ");
            unsigned long timeDiff = gasValve[i].timestampOff - nowCheck;
            Serial.println(timeDiff);
          }
          
        gasValve[i].timestampOn = nowCheck; // NOTE: timestampON!
      }//end of oFFDuration
    }
  }

  
}

