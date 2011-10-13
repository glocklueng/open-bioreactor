void gasValvesSetup();
void gasValvesCH4TurnOff();
void gasValvesCH4TurnOn();
int gasValvesCH4GetState();
void gasValvesCO2TurnOff();
void gasValvesCO2TurnOn();
int gasValvesCO2GetState();
void gasValvesN2TurnOff();
void gasValvesN2TurnOn();
int gasValvesN2GetState();


int gasValvesCH4State = 0;
int gasValvesCO2State = 0;
int gasValvesN2State = 0;


void gasValvesSetup()
{
   pinMode(PIN_GAS_CH4, OUTPUT);
   pinMode(PIN_GAS_CO2, OUTPUT);
   pinMode(PIN_GAS_N2, OUTPUT);
}

//--------- Gas CH4 --------
void gasValvesCH4TurnOff()
{
   digitalWrite(PIN_GAS_CH4, LOW);
   gasValvesCH4State = 0; 
  
}

void gasValvesCH4TurnOn()
{
   digitalWrite(PIN_GAS_CH4, HIGH); 
   gasValvesCH4State = 1;
}

int gasValvesCH4GetState()
{
 return gasValvesCH4State; 
}

//--------- Gas CO2 --------
void gasValvesCO2TurnOff()
{
   digitalWrite(PIN_GAS_CO2, LOW);
   gasValvesCO2State = 0; 
  
}

void gasValvesCO2TurnOn()
{
   digitalWrite(PIN_GAS_CO2, HIGH); 
   gasValvesCO2State = 1;
}

int gasValvesCO2GetState()
{
 return gasValvesCO2State; 
}

//--------- Gas N2 --------
void gasValvesN2TurnOff()
{
   digitalWrite(PIN_GAS_N2, LOW);
   gasValvesN2State = 0; 
  
}

void gasValvesN2TurnOn()
{
   digitalWrite(PIN_GAS_N2, HIGH); 
   gasValvesN2State = 1;
}

int gasValvesN2GetState()
{
 return gasValvesN2State; 
}
