#include <stdio.h>
#include <stdlib.h>

//#include <SD.h>
#include <OneWire.h>
#include <PID_v1.h>
#include <TimedAction.h>

#include <SPI.h> // needed for Ethernet library
#include <Ethernet.h>
#include <EthernetUdp.h> // in the libraries/Ethernet folder
#include <DallasTemperature.h>



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
#define BIOREACTOR_ERROR_MODE 99

#define WAIT_TIME_BEFORE_PUMPING_OUT_MIN 60 // in [sec]
#define WAIT_TIME_BEFORE_PUMPING_OUT_MAX 3600//in [sec]; 1 hour ; stack overflow if unsigned long used
#define SEC_TO_MILLISEC 1000 // in order to obtain millisec (required by timer) 
#define MIN_TO_SEC 60

//----------CONSTANTS-HEATING---------
#define PIN_HEATING_RESISTANCE 28//or 7 if on PWM
//for the regulation of temperature values btw 10 and 45 [s] are commun
#define HEATING_REGULATION_TIME_WINDOWS 5000 //in [ms] 
#define HEATING_MAX_ALLOWED_LIMIT 40.0f // MIN and MAX allowed input values to set the temperature over webUI 
// f at the end means float value 
#define HEATING_MIN_ALLOWED_LIMIT 20.0f // usually below the ambient temperature of the lab

//----------CONSTANTS-LIQUID-LEVEL---------
#define PIN_LIQUID_LEVEL A5
#define LIQUID_LEVEL_PHYSICAL_MAX 8.6f //in inch
#define LIQUID_LEVEL_PHYSICAL_MAX_INTERVAL 1023 //Arduino interval from 0 to 1023
#define LIQUID_LEVEL_PHYSICAL_MIN_INTERVAL 0
#define PIN_FLOAT_MAX A3
#define PIN_FLOAT_MIN A4

//----------CONSTANTS-LOGGING---------  
#define LOG_MAX_LENGTH 256 // in bytes  

//----------CONSTANTS-RELAY-SWITCH---------  
#define PIN_MOTOR 25
#define PIN_PUMP_OUT 26
#define PIN_PUMP_IN 27

//----------CONSTANTS-GAS-VALVES---------
#define PIN_GAS_CH4 39 //methane; closest to the external power supply on the extension board
#define PIN_GAS_CO2 38 //carbonDioxide
#define PIN_GAS_N2 37 //nitrogen

#define OFF 0
#define ON 1
#define CH4 0
#define CO2 1
#define N2 2
#define ALL 10


//----------CONSTANTS-PH-METER--------- 
#define PIN_PH_METER A1
#define pH0 0  // low pH means acidic
#define pH14 818 // high pH means basic/alkaline
// 818 = 819.2-1; corresponds to 4Volts of maximum output of the pH meter

//----------CONSTANTS-TEMPERATURE---------
#define PIN_TEMPERATURE_SENSOR_BOTTOM 33 // most closly to the external power supply of the extension board
#define PIN_TEMPERATURE_SENSOR_IN_LIQUID 32 
#define PIN_TEMPERATURE_SENSOR_AMBIENT 31
#define TEMPERATURE_MIN_ALLOWED_LIMIT 15 // 15°C is the lowest allowed measured voltage; this Bioreactor is made for Cali, Colombia
//#define LOG_TEMPERATURE_LENGTH 512// in bytes


//----------GLOBAL VARIABLES--------- 
//global variables used by functions and are set by WebUI
boolean DEBUG = true;
//limits set by user per WebUI
float HEATING_TEMPERATURE_LIMIT; 
float LIQUID_LEVEL_WEB_MAX; // in arduino AD-intervals [0;1023]
float LIQUID_LEVEL_WEB_MIN;
float pH_SET;
int BIOREACTOR_MODE;
unsigned long WAIT_TIME_BEFORE_PUMPING_OUT; // in [min]
unsigned long PUMPING_OUT_TIMER = 5000; // in [millisec] exceptional definition: because of timer declaration

int bioreactorAncientMode; // variable to detect a unique modechange


//----------TIMER DECLARATION--------- 
TimedAction timerUpdateSensors = TimedAction(10000,globalUpdateSensors); // value is in millisec
TimedAction timerPumpingOut = TimedAction(PUMPING_OUT_TIMER,relaySwitchPumpOutTurnOn); //wait a predefined (by WebUI) time before pumping out (for the sedimentation of bacterias)
TimedAction timerGetCommandPushLog = TimedAction(7000,globalGetCommandAndPushLog);
TimedAction timerSyncNTP = TimedAction(3600000,ethernetSyncNTPTime); // sync NTP time from server every hour



void setup()
{
  // start serial port
  Serial.begin(9600);

  //SD card and the Ethernet chip use the same SPI bus for communication with the Ardnuino
  //only one can be active at a time; the pins are fixe
  pinMode(PIN_SD_CARD, OUTPUT); //pin 4 is for the SD card 
  pinMode(PIN_ETHERNET, OUTPUT); //and pin 10 for the Ethernet chip    

  temperatureSetup();
  heatingSetup();
  relaySwitchSetup();
  gasValvesSetup();
  loggingSetup();
  ethernetSetup();
  //sdCardSetup();
  floatSetup();


  //setup the global variables
  HEATING_TEMPERATURE_LIMIT = 20.0; //initialize with room temperature: no heating at the beginning
  LIQUID_LEVEL_WEB_MAX = 1023; // Arduino internal AD-intervals
  LIQUID_LEVEL_WEB_MIN = 0;
  pH_SET = 7.0;

  //update the epoch time stamp if the NTP server is available  
  ethernetSyncNTPTime();

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
  timerUpdateSensors.check();
  timerSyncNTP.check();
  timerGetCommandPushLog.check();

  // MAIN switch of the Bioreactor
  switch(BIOREACTOR_MODE)
  {
  case BIOREACTOR_MANUAL_MODE:  
    heatingRetulatePIDControl();
    gasValvesCheck();
    break;  

  case BIOREACTOR_STANDBY_MODE: 
    //no heating
    gasValvesCheck();
    break;

  case BIOREACTOR_RUNNING_MODE: 
    heatingRetulatePIDControl();
    gasValvesCheck();
    //automatic program
    
// UNCOMMENTED FOR DEBUGGING PURPOSES    
    if(floatMaxGet() == 0 && floatMinGet() == 1)
    { 
      // ask two times, to avoid artefacts (measurement errors)
      globalUpdateSensors();
      if(floatMaxGet()== 0 && floatMinGet() == 1) 
      { 
        BIOREACTOR_MODE = BIOREACTOR_PUMPING_MODE;  
      } 
    }

    break;

  case BIOREACTOR_PUMPING_MODE:// internal mode
    //wait for for a predefined amount of minutes  
    //and turn on the perilstaltic pump on until a predefined liquid level has been reached    
    timerPumpingOut.check();
    if(floatMaxGet()== 1 && floatMinGet() == 0)
    {  
      // ask two times, to avoid artefacts (measurement errors)
      globalUpdateSensors();
      if(floatMaxGet()== 1 && floatMinGet() == 0)
      {  
        BIOREACTOR_MODE = BIOREACTOR_RUNNING_MODE;
        timerPumpingOut.disable();      
      }
    }
    break;

  case BIOREACTOR_ERROR_MODE: 
    //no heating
    break;

  default:
    Serial.print("ERROR: Bioreactor is in a undefined state! State: ");
    Serial.println(BIOREACTOR_MODE);

  }

  //detect if there is a changement of the mode and do "one time only" commands
  if(bioreactorAncientMode != BIOREACTOR_MODE)  
  {
    bioreactorAncientMode = BIOREACTOR_MODE;
    ethernetPushStatus();
    switch(BIOREACTOR_MODE)
    {
    case BIOREACTOR_MANUAL_MODE:  
      if(DEBUG) Serial.println("SWITCHING BIOREACTOR TO: manual mode.");
      break;

    case BIOREACTOR_STANDBY_MODE:
      if(DEBUG) Serial.println("SWITCHING BIOREACTOR TO: stand-by mode.");      
      //only update sensors and log
      //make sure the motor and pumps are off
      relaySwitchMotorTurnOff();
      relaySwitchPumpOutTurnOff();
      relaySwitchPumpInTurnOff();
      gasValvesTurnOff(ALL);
      break;

    case BIOREACTOR_RUNNING_MODE: 
      if(DEBUG) Serial.println("SWITCHING BIOREACTOR TO: running mode.");
      relaySwitchMotorTurnOn();
      relaySwitchPumpOutTurnOff();
      relaySwitchPumpInTurnOn();
      break;

    case BIOREACTOR_PUMPING_MODE: 
      if(DEBUG) Serial.println("SWITCHING BIOREACTOR TO: pumping mode.");
      //stop the motor for the stearing
      relaySwitchMotorTurnOff();
      relaySwitchPumpInTurnOff();
      gasValvesTurnOff(ALL);
      // turn on PumpOut after a predefined amount of time; done in the MAIN Bioreactor Switch
      timerPumpingOut.enable();
      timerPumpingOut.reset();
      //stop the heating and don't execute the PID algorithm
      digitalWrite(PIN_HEATING_RESISTANCE,LOW); //Heating is OFF  
      break;

    case BIOREACTOR_ERROR_MODE:
      if(DEBUG) Serial.println("SWITCHING BIOREACTOR TO: ERROR mode.");
      //stop EVERYTHING
      relaySwitchMotorTurnOff();
      relaySwitchPumpInTurnOff();
      gasValvesTurnOff(ALL);
      //stop the heating and don't execute the PID algorithm
      digitalWrite(PIN_HEATING_RESISTANCE,LOW); //Heating is OFF       
      break;

    default:
      Serial.print("SWITCHING BIOREACTOR TO: an undefined state! State: ");
      Serial.println(BIOREACTOR_MODE);
    }
  }



}












