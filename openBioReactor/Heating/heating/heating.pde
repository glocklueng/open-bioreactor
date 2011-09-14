#include <OneWire.h>
#include <PID_v1.h>
#include <SD.h>



//----------CONSTANTS---------
#define PIN_TEMPERATURE_SENSOR_BOTTOM 11
#define PIN_TEMPERATURE_SENSOR_IN_LIQUID 9
#define PIN_HEATING_RESISTANCE 12
#define TEMPERATURE_LIMIT 32.0f // f at the end means float value 

//for the regulation of temperature values btw 10 and 45 [s] are commun
#define HEATING_REGULATION_TIME_WINDOWS 5000 //in [ms] 
#define DATA_TAKING_INTERVAL 5000 // in [ms]
#define stringSDwriteLength 1024 // in bytes

bool INITIALIZE;
bool DEBUG = false;
bool DEBUG_TEMP = true;
unsigned long exactPresentTime, timeLastLog, temperatureDebugTimeLastLog;

String tempDataForRegulation = "";


const int chipSelectSDcard = 4; // SD card on pin 4
const int chipSelectEthernet = 10; // Ethernet chip on pin 10

double heatingRegInput, heatingRegOutput, heatingRegSetpoint;
unsigned long heatingRegWindowStartTime;
//Specify the heating regulation links and initial tuning parameters //100; 0.2; 5 are initial testing param.
PID heatingRegPID(&heatingRegInput, &heatingRegOutput, &heatingRegSetpoint, 7000,0,0, DIRECT);  





void setup(void) 
{
  // start serial port
  Serial.begin(9600);
  
  pinMode(PIN_HEATING_RESISTANCE, OUTPUT);
  
  exactPresentTime = millis();
  timeLastLog = exactPresentTime;
  temperatureDebugTimeLastLog = exactPresentTime;
  //write Header for String destined for the .csv file
  tempDataForRegulation = "Milisec,Temperature_HeatingRes, Temperature_inLiquid,PIN_HEATING_RESISTANCE_value\n";
  tempDataForRegulation += "Version: in [ms]";
  tempDataForRegulation += millis();
  tempDataForRegulation += "\n";


  Serial.print("\n\nInitializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(chipSelectSDcard, OUTPUT); //pin 4 is for the SD card 
  pinMode(chipSelectEthernet, OUTPUT); //and pin 10 for the Ethernet chip
  // see if the card is present and can be initialized:
  digitalWrite(chipSelectEthernet, HIGH);
  if (!SD.begin(chipSelectSDcard)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  if(DEBUG) Serial.println("card initialized.");

  //tell the PID to range between 0 and the full window size
  heatingRegPID.SetOutputLimits(0, HEATING_REGULATION_TIME_WINDOWS);
  //turn the PID on
  heatingRegPID.SetMode(AUTOMATIC);
  //set PID sampling time to 1000ms (100ms is default)
  heatingRegPID.SetSampleTime(1000);

  heatingRegWindowStartTime = millis();
  heatingRegSetpoint = TEMPERATURE_LIMIT;


  Serial.println("The Heating-Regulation-PID has been successfully initialized.");   

}





//---------HEATING---------
//Input: Pointer to the OneWire address of the temperature sensor; 
//       Pointer to a place to write the sensor reading; 
//       Pointer to the OneWire object

boolean getOneWireTemp(byte *addr, float *retnTemp, OneWire * OW)
{
  byte present = 0;
  byte data[12];
  int raw;

  OW->reset();
  OW->select(addr);
  OW->write(0x44,0);         // start conversion, with parasite power off at the end

  // NO delay
  //delay(800);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = OW->reset();
  if (present)  //only continue if the sensor is actually there and responding
  {
    OW->select(addr);    
    OW->write(0xBE);         // Read Scratchpad which now contains the temperature data
    for ( int i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = OW->read();
    }
    if ( OneWire::crc8( data, 8) != data[8]) {
      if(DEBUG) Serial.println("CRC is not valid!\n");
      return false;
    }
    raw=(data[1]<<8)+data[0];  //put the two bytes of the temperature (from the response) into a raw int
    *retnTemp = (float)raw * 0.0625; //convert to celcius
    return true;
  }
  return false;
}


char* tempToAscii(float temp)
{
  char ascii[32];
  int frac;
  frac=(unsigned int)(temp*1000)%1000;  //get three numbers to the right of the deciaml point
  itoa((int)temp,ascii,10);
  strcat(ascii,".");
  itoa(frac,&ascii[strlen(ascii)],10); //put the frac after the deciaml

  return ascii;
}



//---------MAIN LOOP---------

void loop(void) 
{
  float tempMeasured, tempMeasuredInLiquid;
  File dataFile;
  
  // DS18S20 Temperature chip i/o
  OneWire ds(PIN_TEMPERATURE_SENSOR_BOTTOM);  // on pin ds(XX)
  OneWire dsInLiquid(PIN_TEMPERATURE_SENSOR_IN_LIQUID);
  //ID of temperature sensor on the bottom of the heating plate
  byte SENSOR_BOTTOM[8]= {
    0x28,0x3F,0xA8,0x23,0x03,0x00,0x00,0xC6        };
  byte SENSOR_IN_LIQUID[8] = {
    0x28,0xD0,0x93,0x64,0x03,0x00,0x00,0x59        };


  if (getOneWireTemp(SENSOR_BOTTOM,&tempMeasured,&ds))
  {
    if(DEBUG_TEMP && (millis() - temperatureDebugTimeLastLog >= DATA_TAKING_INTERVAL))
    {
      Serial.print("The measured temperature on the bottom of the Heating-plate is: ");  
      Serial.println(tempToAscii(tempMeasured));
    }
  }
  else
  {
    Serial.println("ERROR: Temperature on heating resistance could NOT have been measured correctly!");
  }

  if (getOneWireTemp(SENSOR_IN_LIQUID,&tempMeasuredInLiquid,&dsInLiquid))
  {
    if(DEBUG_TEMP &&  (millis() - temperatureDebugTimeLastLog >= DATA_TAKING_INTERVAL))
    {
      Serial.print("The measured temperature in the liquid is: ");  
      Serial.println(tempToAscii(tempMeasuredInLiquid));
      temperatureDebugTimeLastLog = millis();
    }
  }
  else
  {
    Serial.println("ERROR: Temperature in liquid could NOT have been measured correctly!");
  }  



  //----------PID HEATING CONTROL---------
  heatingRegInput = tempMeasuredInLiquid;
  heatingRegPID.Compute();
  

  // turn the output pin on/off based on pid output
  exactPresentTime = millis();
  if(exactPresentTime - heatingRegWindowStartTime > HEATING_REGULATION_TIME_WINDOWS)
  { 
    if(DEBUG) Serial.println("In PID algorithm.");
    //time to shift the Relay Window
    heatingRegWindowStartTime += HEATING_REGULATION_TIME_WINDOWS;
    if(DEBUG) Serial.println(heatingRegWindowStartTime);
  }
  if(DEBUG)
  {
    Serial.print("exactPresentTime");
    Serial.println(exactPresentTime);
    Serial.print("heatingRegWindowStartTime");
    Serial.println(heatingRegWindowStartTime);
    Serial.print("HEATING_REGULATION_TIME_WINDOWS");
    Serial.println(HEATING_REGULATION_TIME_WINDOWS);

    Serial.print("heatingRegOutput");
    Serial.println(heatingRegOutput);
    Serial.print("heatingRegInput");
    Serial.println(heatingRegInput);
  }
  if(heatingRegOutput > exactPresentTime - heatingRegWindowStartTime) 
  {
    digitalWrite(PIN_HEATING_RESISTANCE,HIGH);
    if(DEBUG) Serial.print("Heating...\n");
  }  
  else 
  {
    digitalWrite(PIN_HEATING_RESISTANCE,LOW);
    if(DEBUG) Serial.print("Heating is OFF!\n");      
  }


  //Datalogging
  exactPresentTime = millis();
  if(exactPresentTime - timeLastLog >= DATA_TAKING_INTERVAL)
  {
    tempDataForRegulation += exactPresentTime;
    tempDataForRegulation += ",";
    tempDataForRegulation += tempToAscii(tempMeasured);
    tempDataForRegulation += ",";
    tempDataForRegulation += tempToAscii(tempMeasuredInLiquid);
    tempDataForRegulation += ",";
    tempDataForRegulation += digitalRead(PIN_HEATING_RESISTANCE);
    tempDataForRegulation += "\n";

    if(DEBUG) Serial.println("Taking log datapoint.");
    timeLastLog = exactPresentTime;

    if(DEBUG) 
    {
      Serial.println("The size of the Temperature data String is: ");
      Serial.println(tempDataForRegulation.length());  
    }

    if(tempDataForRegulation.length() >= stringSDwriteLength) // write to SD card.
    {
      if(DEBUG)
      {
        Serial.println("The String buffer is going to be written on the SD card.");
        Serial.println("The dataString is: \n\n");
        Serial.println(tempDataForRegulation);
        Serial.print("The file to be written already exists (1) or is inexistent (0): ");
        int i = SD.exists("/PIDreg.txt");
        Serial.println(i);
      }

      dataFile = SD.open("/PIDreg.txt", FILE_WRITE); // always include the forward slash if data is in root folder

      // if the file is available, write to it:
      if (dataFile) 
      {
        if(DEBUG) Serial.println("Writing to SD card...");
        dataFile.print(tempDataForRegulation); // or string
        dataFile.close();
        // print to the serial port too:
        if(DEBUG) 
        {    
          Serial.println("The data written on the SD card is:");      
          Serial.println(tempDataForRegulation); 
          Serial.println("Finished writing to SD card.");
        }
        Serial.println("The Data is written on the SD card.");

        //retinitaliye String
        tempDataForRegulation = "";
//        //retinitaliye String
//        tempDataForRegulation = "Milisec,Temperature_HeatingRes, Temperature_inLiquid,PIN_HEATING_RESISTANCE_value\n";
//        tempDataForRegulation += "Version: in [ms]";
//        tempDataForRegulation += millis();
//        tempDataForRegulation += "\n";

      }  
      // if the file isn't open, pop up an error:
      else 
      {
        Serial.println("ERROR: opening datafile for writing!");
      } 



    }

  }
  else
  {
    // do nothing
  }



}







