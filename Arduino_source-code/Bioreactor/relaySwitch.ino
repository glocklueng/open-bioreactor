void relaySwitchSetup();
void relaySwitchMotorTurnOff();
void relaySwitchMotorTurnOn();
int relaySwitchMotorGetState();
void relaySwitchMotorSwitchState();
void relaySwitchPumpOutTurnOff();
void relaySwitchPumpOutTurnOn();
void relaySwitchPumpInTurnOff();
void relaySwitchPumpInTurnOn();
int relaySwitchPumpGetAutoSwitchInterval(int pumpID);
void relaySwitchPumpSetAutoSwitchInterval(int pumpID, int percent);
//int relaySwitchPumpGetState(int pumpID);
void relaySwitchAutoSwitchMode(int pumpID);
void relaySwitchCheck();


int relaySwitchMotorState = 0; // 1 if ON; 0 if OFF; other values reserved for error states


struct relaySwitchPumpStruct { 
  int state;
  int autoSwitchInterval; // in [%] percent [0:100]
  boolean autoSwitchFlag;
  int autoSwitchState;
  int arduinoPin;  
  unsigned long timestampOn;
  unsigned long timestampOff;
};

struct relaySwitchPumpStruct pump[2];

void relaySwitchSetup()
{
   pinMode(PIN_MOTOR, OUTPUT);
   pinMode(PIN_PUMP_OUT, OUTPUT);
   pinMode(PIN_PUMP_IN, OUTPUT);
   
   for(int i=0; i<=2; i++)
   {
    pump[i].state = ON;
    pump[i].autoSwitchInterval = 0; // percent 
    pump[i].autoSwitchFlag = true;     
    pump[i].autoSwitchState = ON;
    pump[i].timestampOn = 0;
    pump[i].timestampOff = 0; 
   }
   //NOTE: PUMP_IN 0; PUMP_OUT 1
   pump[PUMP_IN].arduinoPin = PIN_PUMP_IN;
   pump[PUMP_OUT].arduinoPin = PIN_PUMP_OUT;
   
   
  if(DEBUG)Serial.println("Relay switches (pumps and motor) succesfully initialized.");
}

void relaySwitchMotorTurnOff()
{
  //The relaySwitchMotor is connected to the relay in "normally off" mode
  //The relay is switched on HIGH = 5V and the relaySwitchMotor turns ON
   digitalWrite(PIN_MOTOR, HIGH);
   if(DEBUG)Serial.println("The motor has been turned OFF.");
   relaySwitchMotorState = 0; 
  
}

void relaySwitchMotorTurnOn()
{
    //The relaySwitchMotor is connected to the relay in "normally off" mode
  //The relay is switched on LOW = 0V and the relaySwitchMotor turns ON
   digitalWrite(PIN_MOTOR, LOW); 
   if(DEBUG)Serial.println("The motor has been turned ON.");
   relaySwitchMotorState = 1;
}

int relaySwitchMotorGetState()
{
 return relaySwitchMotorState; 
}

void relaySwitchMotorSwitchState()
{
  if(relaySwitchMotorState == 1) 
  {
    digitalWrite(PIN_MOTOR, LOW);
    if(DEBUG)Serial.println("Motor is ON.");
    relaySwitchMotorState = 0;  
    
  }
  else if(relaySwitchMotorState == 0)
 {
    digitalWrite(PIN_MOTOR, HIGH);
    if(DEBUG)Serial.println("Motor is OFF.");
        relaySwitchMotorState = 1;
   
 }
 else Serial.println("WARNING: The Motor is malfunctioning");
 
 
  Serial.println("MotorState has been inversed.");
}



//---------Pump Out---------
void relaySwitchPumpOutTurnOff()
{
//  //PumpOut is connected to the relay in "normally off" mode
//  //The relay is switched on HIGH = 5V and pumpOut turns OFF
//   digitalWrite(PIN_PUMP_OUT, HIGH);

   pump[PUMP_OUT].autoSwitchInterval = 0;
   if(DEBUG)Serial.println("The PumpOut has been turned OFF.");
   
}

void relaySwitchPumpOutTurnOn()
{
//    //PumpOut is connected to the relay in "normally off" mode
//  //The relay is switched on LOW = 0V and pumpOut turns ON
//   digitalWrite(PIN_PUMP_OUT, LOW); 

   pump[PUMP_OUT].autoSwitchInterval = 100;
   if(DEBUG)Serial.println("The pumpOut has been turned ON.");
   
}

//---------Pump In---------
void relaySwitchPumpInTurnOff()
{
//  //PumpIn is connected to the relay in "normally off" mode
//  //The relay is switched on HIGH = 5V and pumpIn turns OFF
//   digitalWrite(PIN_PUMP_IN, HIGH);

   pump[PUMP_IN].autoSwitchInterval = 0;
   if(DEBUG)Serial.println("The pumpIn has been turned OFF.");
  
}

void relaySwitchPumpInTurnOn()
{
//    //PumpIn is connected to the relay in "normally off" mode
//  //The relay is switched on LOW = 0V and pumpIn turns ON
//   digitalWrite(PIN_PUMP_IN, LOW);

   pump[PUMP_IN].autoSwitchInterval = 100;
   if(DEBUG)Serial.println("The pumpIn has been turned ON."); 
}



//---------Pump AutoSwitching mode---------

//int gasValvesGetState(int gasValveID)
//{
//  if (pumpID == PUMP_IN
//    || pumpID == PUMP_OUT)    
//  {
//    return gasValve[gasValveID].state;
//  }
//  else
//  {
//    Serial.println("ERROR: Invalid gas valve ID! Gas valve's state cannot be returned.");
//    return -1;   
//  }    
//}


int relaySwitchPumpGetAutoSwitchInterval(int pumpID) // GET
{
  if (pumpID == PUMP_IN
    || pumpID == PUMP_OUT)    
  {
    return pump[pumpID].autoSwitchInterval;
  }
  else
  {
    Serial.println("ERROR: Invalid pump ID! Pump's AutoSwitchInterval cannot be returned.");
    return -1;   
  }    
}

void relaySwitchPumpSetAutoSwitchInterval(int pumpID, int percent) // SET
{
  if (pumpID == PUMP_IN
    || pumpID == PUMP_OUT)    
  {
    //preliminary checking of the extreme values
    if(percent <= 0)
    {
      pump[pumpID].autoSwitchInterval = 0; //security: if an invalid number has been saved over webUI
    }
    else if(percent >= 100)
    {
      pump[pumpID].autoSwitchInterval = 100; //security: if an invalid number has been saved over webUI
    }
    else
    {      
      pump[pumpID].autoSwitchInterval = percent;
    }   

  }//end of checking for right valve ID
  else
  {
    Serial.println("ERROR: Invalid pump ID! Pump's AutoSwitchInterval cannot be saved.");
  }
}

void relaySwitchAutoSwitchMode(int pumpID)
{
  //check if pump is turned on over WebUI
  //if so, put the pump in auto-switching mode and start the timing
  if(pump[pumpID].state == ON)
  {
    pump[pumpID].autoSwitchFlag = true;
    pump[pumpID].autoSwitchState = ON;
    pump[pumpID].timestampOn = millis();
    pump[pumpID].timestampOff = millis();
  }
  else
  {
    pump[pumpID].autoSwitchFlag = false; //turn off flag if OFF over webUI
  }

  if(pump[pumpID].state == ON && pump[pumpID].autoSwitchFlag == true)
  {
    if(DEBUG)Serial.println("Pump auto-switching mode is being called successfully."); 
    relaySwitchCheck();
  }
  else
  {
    Serial.print("WARNING: The pump is turned off in the WebUI. ");
    Serial.println("In order to use the auto-switching mode, first turn ON the valve from the webUI.");
  }

}


// this function is called as well by the main Bioreactor loop
void relaySwitchCheck()
{
  unsigned long nowCheck;
  unsigned long offDuration;
  unsigned long onDuration;
  unsigned long timeInterval = 60000; // equals 1 min

  /*Note: as defined in main: 
   #define PUMP_IN 0
   #define PUMP_OUT 1
   */

  for(int i=0; i<=1; i++)
  {
    //first check if the two extreme values have been saved in autoSwitchInterval (0 or 100%)
    if(pump[i].autoSwitchInterval == 0)
    {
      if(digitalRead(pump[i].arduinoPin != HIGH))
      {
        digitalWrite(pump[i].arduinoPin, HIGH); // turn off (put HIGH on pin; gasValves need LOW on pin!)
      }
      pump[i].autoSwitchState = OFF;
    }
    else if (pump[i].autoSwitchInterval == 100)
    {
      if(digitalRead(pump[i].arduinoPin != LOW))
      {
        digitalWrite(pump[i].arduinoPin, LOW); // turn on      
      }
      pump[i].autoSwitchState = ON;
    }  


    else if(pump[i].autoSwitchFlag == true)
    {
      onDuration = pump[i].autoSwitchInterval * timeInterval / 100; // devide by 100 because the % is stored as INT
      offDuration = timeInterval - onDuration;
      nowCheck = millis();
      if(DEBUG)Serial.println("Checking pumps for auto-switching mode. Flag-ON & interval calculated.");
      if((pump[i].autoSwitchState == ON) && ((nowCheck - pump[i].timestampOn) >= onDuration))
      {
        if(DEBUG)Serial.println("Checking pumps for auto-switching mode. In autoSwitchState == ON state");
        digitalWrite(pump[i].arduinoPin, HIGH); //switch off
        pump[i].autoSwitchState = OFF;
        if(DEBUG)
        {
          Serial.print("The overtime for valve with ID: ");
          Serial.print(i);
          Serial.print(" being ON is [millisec]: ");
          unsigned long timeDiff = pump[i].timestampOn - nowCheck;
          Serial.println(timeDiff);
        }

        pump[i].timestampOff = nowCheck; // NOTE: timestampOFF!
      }//end of onDuration

      if((pump[i].autoSwitchState == OFF) && ((nowCheck - pump[i].timestampOff) >= offDuration))
      {
        if(DEBUG)Serial.println("Checking pumps for auto-switching mode. In autoSwitchState == OFF state");
        digitalWrite(pump[i].arduinoPin, LOW); //switch on
        pump[i].autoSwitchState = ON;
        if(DEBUG)
        {
          Serial.print("The overtime for valve with ID: ");
          Serial.print(i);
          Serial.print(" being OFF is [millisec]: ");
          unsigned long timeDiff = pump[i].timestampOff - nowCheck;
          Serial.println(timeDiff);
        }

        pump[i].timestampOn = nowCheck; // NOTE: timestampON!
      }//end of oFFDuration
    }
  } // end of   for(int i=0; i<=2; i++)


}

