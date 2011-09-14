//----------CONSTANTS---------
int pinPHsensor = A11;
int pH0 = 0;  // low pH means acidic
int pH14 = 1023; // high pH means basic/alkaline

bool DEBUG = true;




void setup(void) {
  // initialize inputs/outputs
  // start serial port
  Serial.begin(9600);
}

//---------MAIN LOOP---------

void loop(void) 
{
  int pHLevel;
  
  pHLevel = analogRead (pinPHsensor);

  
  
  if(DEBUG)
  {
    Serial.print("pH of the liquid in AD-intervals (Arduino): ");
    Serial.println(pHLevel);  
    Serial.print("pH in MiliVolts: ");
    Serial.println(pHLevel*4.88);
    if(pHLevel*4.88 + 50 )  
  }
  
  delay(1000);
  
  
}
