void liquidLevelUpdate();
int liquidLevelGet();
void floatSetup();
void floatUpdate();
void floatMaxRead();
void floatMinRead();
int floatMinGet();
int floatMaxGet();
int floatMaxValue;
int floatMinValue;

// Hight Vout values if reactor is empty
// see eTape (PN-6573P-8) datasheet for more information
// originally calibrated values: 353 if full and 714 if empty 

//float liquidSensorLength = 21.84; // [cm] according to datasheet


int liquidInterval;       // gloabl variable for export; read value of the analog Pin [0;1023]
                          // the exact height and the volume are calculated on the server


void liquidLevelUpdate() 
{
  liquidInterval = analogRead(PIN_LIQUID_LEVEL); // the calibrated lookup table is on the web/log-server;
  
  // the conversation is done on the server for the exact liquid level and the liquid volume
  // only the read analog value of the pin is uploaded to the server.
  
}

int liquidLevelGet()
{
  return liquidInterval;
}

void floatSetup()
{
 pinMode(PIN_FLOAT_MAX, INPUT);
 pinMode(PIN_FLOAT_MIN, INPUT);
  
}

void floatUpdate()
{
floatMaxRead();
floatMinRead();

}
void floatMaxRead()
{
int floatMaxValue = digitalRead(PIN_FLOAT_MAX); //is 0 when float is up, 1 when down
if(DEBUG)
{
  Serial.print("The floatMaxValue is: ");
  Serial.println(floatMaxValue);
}
}

void floatMinRead()
{
int floatMinValue = digitalRead(PIN_FLOAT_MIN); //is 1 when float is up, o when down
if(DEBUG)
{
  Serial.print("The floatMinValue is: ");
  Serial.println(floatMinValue);
}
}

int floatMaxGet()
{
floatMaxRead();
return floatMaxValue; 
}  

int floatMinGet()
{
floatMinRead();  
return floatMinValue; 
}  
