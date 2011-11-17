void gasValvesSetup();
void gasValvesTurnOff(int gasValveID);
void gasValvesTurnOn(int gasValveID);
int gasValvesGetState(int gasValveID);
int gasValvesGetAutoSwitchInterval(int gasValveID);
void gasValvesSetAutoSwitchInterval(int gasValveID, int percent);
void gasValvesAutoSwitchMode();
void gasValvesCheck();

struct gasValveStruct { 
  int state;
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

  /*Note: as defined in main: 
   #define CH4 0
   #define CO2 1
   #define N2 2
   */

  for(int i=0; i<=2; i++)
  {
    gasValve[i].state = OFF;
    gasValve[i].autoSwitchInterval = 0; // percent 
    gasValve[i].autoSwitchFlag = false;     
    gasValve[i].autoSwitchState = OFF;
    gasValve[i].timestampOn = 0;
    gasValve[i].timestampOff = millis(); 
  }

  gasValve[CH4].arduinoPin = PIN_GAS_CH4;
  gasValve[CO2].arduinoPin = PIN_GAS_CO2;
  gasValve[N2].arduinoPin = PIN_GAS_N2;

  if(DEBUG)Serial.println("Gas Valves succesfully initialized.");
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
    Serial.println("Debug_8");
  if (gasValveID == CH4
    || gasValveID == CO2
    || gasValveID == N2)
  {
      Serial.println("Debug_9");
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
      Serial.println("Debug_7");
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
      Serial.println("Debug_6");
  if (gasValveID == CH4
    || gasValveID == CO2
    || gasValveID == N2)    
  {
    //preliminary checking of the extreme values
    if(percent <= 0)
    {
      gasValve[gasValveID].autoSwitchInterval = 0; //security: if an invalid number has been saved over webUI
    }
    else if(percent >= 100)
    {
      gasValve[gasValveID].autoSwitchInterval = 100; //security: if an invalid number has been saved over webUI
    }
    else
    {      
      gasValve[gasValveID].autoSwitchInterval = percent;
    }   

  }//end of checking for right valve ID
  else
  {
    Serial.println("ERROR: Invalid gas valve ID! Gas valve's AutoSwitchInterval cannot be saved.");
  }
}

void gasValvesAutoSwitchMode(int gasValveID)
{
      Serial.println("Debug_5");
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
    if(DEBUG)Serial.println("Gas valves auto-switching mode is being called successfully."); 
    gasValvesCheck();
  }
  else
  {
    Serial.print("WARNING: The gas valve is turned off in the WebUI. ");
    Serial.println("In order to use the auto-switching mode, first turn ON the valve from the webUI.");
  }

}


// this function is called as well by the main Bioreactor loop
void gasValvesCheck()
{
      Serial.println("Debug_4");
  unsigned long nowCheck;
  unsigned long offDuration;
  unsigned long onDuration;
  unsigned long timeInterval = 60000; // equals 1 min

  /*Note: as defined in main: 
   #define CH4 0
   #define CO2 1
   #define N2 2
   */

  for(int i=0; i<=2; i++)
  {
    if(gasValve[i].autoSwitchFlag == true)
    {
      onDuration = gasValve[i].autoSwitchInterval / 100 * timeInterval; // devide by 100 because the % is stored as INT
      offDuration = timeInterval - onDuration;
      nowCheck = millis();
    if(DEBUG)Serial.println("Checking gas valves for auto-switching mode. Flag-ON & interval calculated.");
      if((gasValve[i].autoSwitchState == ON) && ((nowCheck - gasValve[i].timestampOn) >= onDuration))
      {
        if(DEBUG)Serial.println("Checking gas valves for auto-switching mode. In autoSwitchState == ON state");
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
        if(DEBUG)Serial.println("Checking gas valves for auto-switching mode. In autoSwitchState == OFF state");
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


