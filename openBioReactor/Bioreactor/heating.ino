void heatingSetup();
void heatingRetulatePIDControl();
void heatingDebug();
void heatingSecurityAlgorithm();
void heatingDebug();
int heatingPIDRegOutput();


/********************************************************
 * PID RelayOutput Example from PID library 1.0.1
 * Same as basic example, except that this time, the output
 * is going to a digital pin which (we presume) is controlling
 * a relay.  The pid is designed to output an analog value,
 * but the relay can only be On/Off.
 *
 *   To connect them together we use "time proportioning
 * control"  Tt's essentially a really slow version of PWM.
 * First we decide on a window size (5000mS say.) We then 
 * set the pid to adjust its output between 0 and that window
 * size.  Lastly, we add some logic that translates the PID
 * output into "Relay On Time" with the remainder of the 
 * window being "Relay Off Time"
 ********************************************************/

double heatingRegInput, heatingRegOutput, heatingRegSetpoint;
unsigned long heatingRegWindowStartTime;
//Specify the heating regulation links and initial tuning parameters //Kp=100; Ti=0.2; Td=5 are initial testing param.
PID heatingRegPID(&heatingRegInput, &heatingRegOutput, &heatingRegSetpoint, 7000,15,300, DIRECT);  



void heatingSetup()
{
  pinMode(PIN_HEATING_RESISTANCE, OUTPUT);
  //tell the PID to range between 0 and the full window size
  heatingRegPID.SetOutputLimits(0, HEATING_REGULATION_TIME_WINDOWS);
  //turn the PID on
  heatingRegPID.SetMode(AUTOMATIC);
  //set PID sampling time to 10000ms 
  //Usual sampling time for temperature applications is btw 10s and 45s
  //according to "Commande numérique de systèmes dynamiques 
  // - Cours d'automatique" by R. Longchamp
  heatingRegPID.SetSampleTime(10000);

  heatingRegWindowStartTime = millis();
  heatingRegSetpoint = HEATING_TEMPERATURE_LIMIT;

  Serial.println("The Heating-Regulation-PID has been successfully initialized.");   

  if(DEBUG)
  {
    Serial.print("The temperature limit for PID regulation is set to: ");
    Serial.println(heatingRegSetpoint); 
  } 

}

void heatingRetulatePIDControl()
{
  float exactPresentTime;
  heatingRegInput = temperatureMeasuredInLiquid();
  heatingRegSetpoint = HEATING_TEMPERATURE_LIMIT;
  heatingRegPID.Compute(); // the computation takes only 30ms!

  // turn the output pin on/off based on pid output
  exactPresentTime = millis();
  if(exactPresentTime - heatingRegWindowStartTime > HEATING_REGULATION_TIME_WINDOWS)
  { 
    if(DEBUG) 
    {
      Serial.print("In PID algorithm.\t");
      Serial.print("The PID internal out put is: ");
      Serial.println(heatingRegOutput);
    }
    //time to shift the Relay Window
    heatingRegWindowStartTime += HEATING_REGULATION_TIME_WINDOWS;
    //if(DEBUG) Serial.println(heatingRegWindowStartTime);
  }

  if(heatingRegOutput > exactPresentTime - heatingRegWindowStartTime) 
  {
    digitalWrite(PIN_HEATING_RESISTANCE,HIGH);
    //if(DEBUG) Serial.print("Heating...\n");
  }  
  else 
  {
    digitalWrite(PIN_HEATING_RESISTANCE,LOW);
    //if(DEBUG) Serial.print("Heating is OFF!\n");      
  } 

}

void heatingSecurityAlgorithm()
{
  // security algorithm: if the temperature of the bottom plate is higher than HEATING_MAX_ALLOWED_LIMIT degrees; 
  // turn heating of and put bioreactor in STAND BY state
  // if the measured temperature is lower than TEMPERATURE_MIN_ALLOWED_LIMIT, that means sensor is working WRONG! 
  if(temperatureMeasuredBottomPlate() > HEATING_MAX_ALLOWED_LIMIT
    || temperatureMeasuredInLiquid() > HEATING_MAX_ALLOWED_LIMIT
    || temperatureMeasuredInLiquid() < TEMPERATURE_MIN_ALLOWED_LIMIT
    || temperatureMeasuredBottomPlate() < TEMPERATURE_MIN_ALLOWED_LIMIT
    || temperatureMeasuredAmbient() < TEMPERATURE_MIN_ALLOWED_LIMIT
    )
  {
    //override all important internal heating commands!
    digitalWrite(PIN_HEATING_RESISTANCE,LOW);
    heatingRegInput = 1; // higher than Setpoint to avoid heating
    heatingRegSetpoint = 0;
    BIOREACTOR_MODE = BIOREACTOR_ERROR_MODE;
    Serial.println("WARNING: The temperature is outside the range! The bioreactor has been forced to standby mode!");
    heatingDebug(); 

  }

}

void heatingDebug() {
  Serial.print("HEATING_MAX_ALLOWED_LIMIT: ");
  Serial.println(HEATING_MAX_ALLOWED_LIMIT); 
  Serial.print("TEMPERATURE_MIN_ALLOWED_LIMIT: ");
  Serial.println(TEMPERATURE_MIN_ALLOWED_LIMIT); 
  Serial.print("temperatureMeasuredBottomPlate: ");
  Serial.println(temperatureMeasuredBottomPlate()); 
  Serial.print("temperatureMeasuredInLiquid: ");
  Serial.println(temperatureMeasuredInLiquid()); 
  Serial.print("temperatureMeasuredAmbient: ");
  Serial.println(temperatureMeasuredAmbient()); 
}

int heatingPIDRegOutput()
{
  return heatingRegOutput;
}


