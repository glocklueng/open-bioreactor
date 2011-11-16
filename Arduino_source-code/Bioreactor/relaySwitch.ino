void relaySwitchMotorSetup();
void relaySwitchMotorTurnOff();
void relaySwitchMotorTurnOn();
int relaySwitchMotorGetState();
void relaySwitchMotorSwitchState();
void relaySwitchPumpOutTurnOff();
void relaySwitchPumpOutTurnOn();
int relaySwitchPumpOutGetState();
void relaySwitchPumpInTurnOff();
void relaySwitchPumpInTurnOn();
int relaySwitchPumpInGetState();


int relaySwitchMotorState = 0; // 1 if ON; 0 if OFF; other values reserved for error states
int relaySwitchPumpOutState = 0;
int relaySwitchPumpInState = 0;


void relaySwitchSetup()
{
   pinMode(PIN_MOTOR, OUTPUT);
   pinMode(PIN_PUMP_OUT, OUTPUT);
   pinMode(PIN_PUMP_IN, OUTPUT);
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
  //PumpOut is connected to the relay in "normally off" mode
  //The relay is switched on HIGH = 5V and pumpOut turns OFF
   digitalWrite(PIN_PUMP_OUT, HIGH);
   if(DEBUG)Serial.println("The PumpOut has been turned OFF.");
   relaySwitchPumpOutState = 0; 
  
}

void relaySwitchPumpOutTurnOn()
{
    //PumpOut is connected to the relay in "normally off" mode
  //The relay is switched on LOW = 0V and pumpOut turns ON
   digitalWrite(PIN_PUMP_OUT, LOW); 
   if(DEBUG)Serial.println("The pumpOut has been turned ON.");
   relaySwitchPumpOutState = 1;
}

int relaySwitchPumpOutGetState()
{
 return relaySwitchPumpOutState; 
}

//---------Pump In---------
void relaySwitchPumpInTurnOff()
{
  //PumpIn is connected to the relay in "normally off" mode
  //The relay is switched on HIGH = 5V and pumpIn turns OFF
   digitalWrite(PIN_PUMP_IN, HIGH);
   if(DEBUG)Serial.println("The pumpIn has been turned OFF.");
   relaySwitchPumpInState = 0; 
  
}

void relaySwitchPumpInTurnOn()
{
    //PumpIn is connected to the relay in "normally off" mode
  //The relay is switched on LOW = 0V and pumpIn turns ON
   digitalWrite(PIN_PUMP_IN, LOW);
     if(DEBUG)Serial.println("The pumpIn has been turned ON."); 
   relaySwitchPumpInState = 1;
}

int relaySwitchPumpInGetState()
{
 return relaySwitchPumpInState; 
}

