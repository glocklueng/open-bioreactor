//----------CONSTANTS---------
int pinLiquidLevel = A11;
int liquidLevelFull = 363;  // when the reactor is full
int liquidLevelEmpty = 716; // when the reactor is empty (no liquid up to 1 inch of liquid)
                            // Hight Vout values if reactor is empty
                            // see eTape (PN-6573P-8) datasheet for more information
                            // originally calibrated values: 353 if full and 714 if empty 
float liquidSensorLength = 21.84; // [cm] according to datasheet
float reservoirDiameter = 20.5; // [cm] !!only approx value


bool DEBUG = true;


void setup(void) {
  // initialize inputs/outputs
  // start serial port
  Serial.begin(9600);
}

//---------MAIN LOOP---------

void loop(void) 
{
  int liquidLevelAtPin;
  float liquidPercentFull, liquidHeight, liquidVolume, errorLiquidbySensor;
  
  liquidLevelAtPin = analogRead (pinLiquidLevel);
  liquidPercentFull = (float)(liquidLevelEmpty-liquidLevelAtPin)/(liquidLevelEmpty-liquidLevelFull); 
  liquidHeight = liquidPercentFull*liquidSensorLength;
  liquidVolume = PI*(reservoirDiameter/2)*(reservoirDiameter/2)*liquidHeight; //in cm^3=mL
  
  if(liquidPercentFull>1.0)Serial.println("ERROR: more than 100% liquid!");
  //Approximate error: 10% on eTape resistance value by sensor (see datasheet)
  //errorLiquidbySensor = 0.1*...;
  
  
  if(DEBUG)
  {
    Serial.print("Liquid level in Volt-interval on Arduino: ");
    Serial.println(liquidLevelAtPin);       
    Serial.print("Liquid level in inch: ");
    Serial.println(liquidHeight);
    Serial.print("Liquid level in cm: ");
    Serial.println(liquidHeight*2.54);
    Serial.print("Liquid level in liters: ");
    Serial.println(liquidVolume/1000);
    Serial.print("Liquid level in percent: ");
    Serial.println(liquidPercentFull); 
  }
  
  delay(1000);
  
  
}
