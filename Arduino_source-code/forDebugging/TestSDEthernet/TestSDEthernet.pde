// INCLUDES MUST BE THE FIRST LINE !!!
// Otherwise the library will not be loaded at the beginning and we will not be able
// to pass pointers to library objects !!!


#include <SD.h>
#include <Ethernet.h>
#include <Udp.h>
#include <SPI.h>

/*
  http://www.nongnu.org/avr-libc/user-manual/group__avr__watchdog.html
  http://www.atmel.com/dyn/resources/prod_documents/doc2551.pdf
  We may need to change the bootloader because by default it will not start on a WDT reset (Software reset) !!!
  http://wiblocks.luciani.org/docs/app-notes/software-reset.html
*/
#include <avr/wdt.h>

#include <EXROM.h>
#include <EEPROM.h>

#include <Servo.h> 

// We need to install also other libraries in Resources/Java/Librairies
// http://www.arduino.cc/playground/Code/Time
#include <Time.h>

// http://www.arduino.cc/playground/Learning/OneWire - allows to communicate with Temperature probes for example
// http://www.pjrc.com/teensy/td_libs_OneWire.html
#include <OneWire.h>
// http://www.milesburton.com/?title=Dallas_Temperature_Control_Library
#include <DallasTemperature.h>

// http://www.arduino.cc/playground/Code/SimpleTimer
#include <SimpleTimer.h>

// http://www.arduino.cc/playground/Main/MsTimer2
// This library will allow to execute a method regularly
#include <MsTimer2.h>

// Number of input / output ports
const int NUMBER_IO=54;

// Number of analogic / digital ports
const int NUMBER_AD=16;

// Pin for the selection of the SD card
const int SD_SELECT = 4;

// Pin for the selection of ethernet card
const int ETHERNET_SELECT = 10;


const byte EVENT_NO=0;
const byte EVENT_BOOT=1;
const byte EVENT_TEMPERATURE_INCREASE=2;
const byte EVENT_TEMPERATURE_STABILITY=3;
const byte EVENT_LEVEL_STABILITY=4;
const byte NTP_SERVER_NOT_FOUND=5;


// With DEBUG on a lot of information is printed to SERIAL
boolean DEBUG=true;

void setup() {
  // we just initialize the Serial object in order to monitor what is going on
  Serial.begin(9600);
  
  // We will engable the watchdog
  // Activating the WDT may put the Arduino in a reset loop
  // To escape from it a way is to try after a power up to upload Blink
  // wdt_enable(WDTO_8S); 
  
  setupEventLogger();
  setupNetwork();
  setupNtpTime();
  updateTime(true);
  setupWebServer();
  setupSD();
  setupTemperature();
  setupSimpleTimer();
  setupInterrupt();
  addEvent(EVENT_BOOT);
}

void loop() {
  wdt_reset();
  loopWebServer();
  loopTemperature();
  loopSimpleTimer();
}


