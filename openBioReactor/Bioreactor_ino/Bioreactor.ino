#include <SD.h>
//#include <OneWire.h>
#include <PID_v1.h>
#include <TimedAction.h>

#include <SPI.h> // needed for Ethernet library
#include <Ethernet.h>
#include <Udp.h> // in the libraries/Ethernet folder



//----------CONSTANTS-GLOBAL---------
#define BIOREACTOR_ID 1 // ID=1 since this is the very first prototype Bioreactor
#define PIN_SD_CARD 4 // SD card on pin 4 (this is not changable)
#define PIN_ETHERNET 10 // Ethernet chip on pin 10 (this is not changable)
// NOTE: only one can be active at a time due to hardware restrictions
// because both use the same SPI bus for communication (Ethernet shield <-> Arduino Main Module)
#define BIOREACTOR_MANUAL_MODE 0
#define BIOREACTOR_STANDBY_MODE 1
#define BIOREACTOR_RUNNING_MODE 2
#define BIOREACTOR_PUMPING_MODE 3

//----------CONSTANTS-HEATING---------
#define PIN_HEATING_RESISTANCE 28//or 7 if on PWM
//for the regulation of temperature values btw 10 and 45 [s] are commun
#define HEATING_REGULATION_TIME_WINDOWS 5000 //in [ms] 
#define HEAGINT_MAX_ALLOWED_LIMIT 40.0f // MIN and MAX allowed input values to set the temperature over webUI 
// f at the end means float value 
#define HEAGINT_MIN_ALLOWED_LIMIT 20.0f // usually below the ambient temperature of the lab

//----------CONSTANTS-LIQUID-LEVEL---------
#define PIN_LIQUID_LEVEL A5
#define LIQUID_LEVEL_MAX 8.6f //in inch

//----------CONSTANTS-LOGGING---------  
#define LOG_MAX_LENGTH 1024 // in bytes  

//----------CONSTANTS-RELAY-SWITCH---------  
#define PIN_MOTOR 25
#define PIN_PUMP_OUT 26
#define PIN_PUMP_IN 27

//----------CONSTANTS-GAS-VALVES---------
#define PIN_GAS_CH4 39 //methane; closest to the external power supply on the extension board
#define PIN_GAS_CO2 38 //carbonDioxide
#define PIN_GAS_N2 37 //nitrogen


//----------CONSTANTS-PH-METER--------- 
#define PIN_PH_METER A1
#define pH0 0  // low pH means acidic
#define pH14 818 // high pH means basic/alkaline
// 818 = 819.2-1; corresponds to 4Volts of maximum output of the pH meter

//----------CONSTANTS-TEMPERATURE---------
#define PIN_TEMPERATURE_SENSOR_BOTTOM 33 // most closly to the external power supply of the extension board
#define PIN_TEMPERATURE_SENSOR_IN_LIQUID 32 
#define PIN_TEMPERATURE_SENSOR_AMBIENT 31
//#define LOG_TEMPERATURE_LENGTH 512// in bytes

TimedAction timerUpdateSensors = TimedAction(5000,globalUpdateSensors); // value is in milisec
TimedAction timerPumpingOut = TimedAction(30000,relaySwitchPumpOutTurnOn); //wait 10min before pumping out
TimedAction timerGetCommandPushLog = TimedAction(2000,globalGetCommandAndPushLog);


//global variables used by functions and are set by WebUI
boolean DEBUG = true;
//limits set by user per WebUI
float HEATING_TEMPERATURE_LIMIT; 
float LIQUID_LEVEL_SET; // in inch
float pH_SET;
int BIOREACTOR_MODE;
int bioreactorAncientMode; // variable to detect a unique modechange



void setup()
{
  // start serial port
  Serial.begin(9600);

  //SD card and the Ethernet chip use the same SPI bus for communication with the Ardnuino
  //only one can be active at a time; the pins are fixe
  pinMode(PIN_SD_CARD, OUTPUT); //pin 4 is for the SD card 
  pinMode(PIN_ETHERNET, OUTPUT); //and pin 10 for the Ethernet chip    

  //temperatureSetup();
  heatingSetup();
  relaySwitchSetup();
  gasValvesSetup();
  //loggingSetup();
  ethernetSetup();
  sdCardSetup();


  //setup the global variables
  HEATING_TEMPERATURE_LIMIT = 31.0; 
  LIQUID_LEVEL_SET = 8.6; // in inch
  pH_SET = 7.0;


  // update all the sensor data for once before doing the first log
  globalUpdateSensors();

  // turn off all the pumps/valves/motor
  relaySwitchMotorTurnOff();
  relaySwitchPumpOutTurnOff();
  relaySwitchPumpInTurnOff();

  //    //put the Bioreactor in running mode
  BIOREACTOR_MODE = BIOREACTOR_RUNNING_MODE;
  bioreactorAncientMode = BIOREACTOR_MODE;

  Serial.println("The Bioreactor has been set up and is running...\n");
}


void loop()
{  
  // MAIN switch of the Bioreactor
  switch(BIOREACTOR_MODE)
  {
  case BIOREACTOR_MANUAL_MODE:  
    heatingRetulatePIDControl();
    timerUpdateSensors.check();
    timerGetCommandPushLog.check();
    break;  

  case BIOREACTOR_STANDBY_MODE: 
    //no heating
    timerUpdateSensors.check();
    timerGetCommandPushLog.check();
    break;

  case BIOREACTOR_RUNNING_MODE: 
    heatingRetulatePIDControl();
    timerUpdateSensors.check();
    timerGetCommandPushLog.check();
    //automatic program
    if(liquidLevelGet()>=8.25)
    { 
      // ask two times, to avoid artefacts (measurement errors)
      globalUpdateSensors();
      if(liquidLevelGet()>=8.25) 
      { 
        BIOREACTOR_MODE = BIOREACTOR_PUMPING_MODE;  
      } 
    }

    break;

  case BIOREACTOR_PUMPING_MODE:// internal mode
    //wait for for a predefined amount of minutes  
    //and turn on the perilstaltic pump on until a predefined liquid level has been reached    
    timerPumpingOut.check();
    timerUpdateSensors.check();
    timerGetCommandPushLog.check();
    if(liquidLevelGet()<=5.75)
    {  
      // ask two times, to avoid artefacts (measurement errors)
      globalUpdateSensors();
      if(liquidLevelGet()<=5.75)
      {  
        BIOREACTOR_MODE = BIOREACTOR_RUNNING_MODE;
        timerPumpingOut.disable();      
      }
    }

    break;

  default:
    Serial.print("ERROR: Bioreactor is in a undefined state! State: ");
    Serial.println(BIOREACTOR_MODE);

  }

  //detect if there is a changement of the mode and do "one time only" commands
  if(bioreactorAncientMode != BIOREACTOR_MODE)  
  {
    switch(BIOREACTOR_MODE)
    {
    case BIOREACTOR_MANUAL_MODE:  
      if(DEBUG) Serial.println("The Bioreactor is in manual mode.");
      bioreactorAncientMode = BIOREACTOR_MODE;
      break;

    case BIOREACTOR_STANDBY_MODE:
      if(DEBUG) Serial.println("The Bioreactor is in stand-by mode.");      
      bioreactorAncientMode = BIOREACTOR_MODE;
      //only update sensors and log
      //make sure the motor and pumps are off
      relaySwitchMotorTurnOff();
      relaySwitchPumpOutTurnOff();
      relaySwitchPumpInTurnOff();

      break;

    case BIOREACTOR_RUNNING_MODE: 
      if(DEBUG) Serial.println("The Bioreactor is in running mode.");
      bioreactorAncientMode = BIOREACTOR_MODE;
      relaySwitchMotorTurnOn();
      relaySwitchPumpOutTurnOff();
      relaySwitchPumpInTurnOn();
      break;

    case BIOREACTOR_PUMPING_MODE: 
      if(DEBUG) Serial.println("The Bioreactor is in pumping mode.");
      bioreactorAncientMode = BIOREACTOR_MODE; 
      //stop the motor for the stearing
      relaySwitchMotorTurnOff();
      relaySwitchPumpInTurnOff(); 
      // turn on PumpOut after a predefined amount of time; done in the MAIN Bioreactor Switch
      timerPumpingOut.enable();
      timerPumpingOut.reset();
      //TODO: stop all inflow of gases

      //stop the heating and don't execute the PID algorithm
      digitalWrite(PIN_HEATING_RESISTANCE,LOW); //Heating is OFF  
      break;

    default:
      Serial.print("ERROR: Bioreactor is in a undefined state! State: ");
      Serial.println(BIOREACTOR_MODE);
    }
  }



}










