void gasValvesSetup();
void gasValvesTurnOff(int gasValveID);
void gasValvesTurnOn(int gasValveID);
int gasValvesGetState(int gasValveID);
int gasValvesGetAutoSwitchInterval(int gasValveID);
void gasValvesSetAutoSwitchInterval(int gasValveID, int percent);
void gasValvesAutoSwitchMode();


struct gasValveStruct { int state;
                        int autoSwitchInterval; // in [%] percent [0:100]
                        int arduinoPin;  
                       };

struct gasValveStruct gasValve[3];

void gasValvesSetup()
{
   pinMode(PIN_GAS_CH4, OUTPUT);
   pinMode(PIN_GAS_CO2, OUTPUT);
   pinMode(PIN_GAS_N2, OUTPUT);
   
   gasValve[CH4].state = OFF;
   gasValve[CH4].autoSwitchInterval = 0; // percent
   gasValve[CH4].arduinoPin = PIN_GAS_CH4;
   gasValve[CO2].state = OFF;
   gasValve[CO2].autoSwitchInterval = 0;
   gasValve[CO2].arduinoPin = PIN_GAS_CO2;
   gasValve[N2].state = OFF;
   gasValve[N2].autoSwitchInterval = 0;
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
       Serial.println("ERROR: Invalid Gas valve ID! Gas valve cannot be turned OFF");
     
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
       Serial.println("ERROR: Invalid Gas valve ID! Gas valve cannot be turned ON");
     
   }    
  
}


int gasValvesGetState(int gasValveID)
{
  if (gasValveID == (CH4 || CO2 || N2))
  {
    return gasValve[gasValveID].state;
  }
  else
  {
    Serial.println("ERROR: Invalid Gas valve ID! Gas valve's state cannot be returned.");
    return -1;   
  }    
}


int gasValvesGetAutoSwitchInterval(int gasValveID) // GET
{
  if (gasValveID == (CH4 || CO2 || N2))
  {
    return gasValve[gasValveID].autoSwitchInterval;
  }
  else
  {
    Serial.println("ERROR: Invalid Gas valve ID!Gas valve's AutoSwitchInterval cannot be returned.");
    return -1;   
  }    
}




void gasValvesSetAutoSwitchInterval(int gasValveID, int percent) // SET
{
    if (gasValveID == (CH4 || CO2 || N2))
    {
      gasValve[gasValveID].autoSwitchInterval = percent;
    }
    else Serial.println("ERROR: Invalid Gas valve ID! Gas valve's AutoSwitchInterval cannot be saved.");
}

void gasValvesAutoSwitchMode()
{
    if(DEBUG)Serial.println("Gas valves auto-switching mode is being called successfully.");
}

