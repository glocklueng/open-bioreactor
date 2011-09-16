/*
We have the way to log some event.
 We will keep in memory the last 20 events
 The events are stored in EEPROM to be sure ...
 An event is characterised by:
 * a timestamp in epoch (long)
 * an eventID 
 * a value
 */

/* EEPROM IS LIMITED TO 100000 WRITES !!!! LOG EVENT WRITE TO EEPROM SO TAKE CARE !!!!!!!
 If the MAX_NUMBER_EVENTS change you need to change the pointer in the EEPROM
 */
#define MAX_NUMBER_EVENTS 32


#define FLOAT_MIN_VALUE  -3.4028235E+38

// Variable are defined in the main class !!!!

static String EVENT_LOG_DESCRIPTION[]={
  "No event",
  "Startup",
  "Suspect temperature increase (in degree / minutes)",
  "Suspect temperature stability after heating",
  "Level does not change after pumping",  
  "Could not connect to NTP server"};

byte eventTypes[MAX_NUMBER_EVENTS];
long eventTimestamps[MAX_NUMBER_EVENTS];
float eventParameters[MAX_NUMBER_EVENTS];

void addEvent(byte eventType) {
  addEvent(eventType, FLOAT_MIN_VALUE);
}

void addEvent(byte eventType, float parameter) {
  if (DEBUG) {
    Serial.print("Logging event: ");
    Serial.print((int)eventType);
    Serial.print(" - Parameter: ");
    Serial.println(parameter);
  }

  // in order to space the EEPROM we only log event if
  // the eventType is different
  // or the elapse time since the last event is > 900s
  // Boot event are still taken into account
  // We however always log the Boot event
  if ((eventType==eventTypes[MAX_NUMBER_EVENTS]) && (eventType>1)) {
    if ((eventTimestamps[MAX_NUMBER_EVENTS]-now())<900) {
      return;
    }
  }

  for (int i=0; i<(MAX_NUMBER_EVENTS-1); i++) {
    eventTypes[i]=eventTypes[i+1];
    eventTimestamps[i]=eventTimestamps[i+1];
    eventParameters[i]=eventParameters[i+1];
  }
  eventTypes[MAX_NUMBER_EVENTS-1]=eventType;
  eventTimestamps[MAX_NUMBER_EVENTS-1]=now();
  eventParameters[MAX_NUMBER_EVENTS-1]=parameter;

  EXROM.write(EE_TARGET_EVENT_TYPES, eventTypes, MAX_NUMBER_EVENTS);
  EXROM.write(EE_TARGET_EVENT_TIMES, eventTimestamps, MAX_NUMBER_EVENTS*4);
  EXROM.write(EE_TARGET_EVENT_PARAMS, eventParameters, MAX_NUMBER_EVENTS*4);
}



void setupEventLogger() {
  // We should load from EEPROM all the events

  EXROM.read(EE_TARGET_EVENT_TYPES, eventTypes, MAX_NUMBER_EVENTS);
  EXROM.read(EE_TARGET_EVENT_TIMES, eventTimestamps, MAX_NUMBER_EVENTS*4);
  EXROM.read(EE_TARGET_EVENT_PARAMS, eventParameters, MAX_NUMBER_EVENTS*4);
}

void getEventLogs(Print* output) {
  for (int i=0; i<MAX_NUMBER_EVENTS; i++) {
    if (eventTypes[i]<sizeof(EVENT_LOG_DESCRIPTION)) {
      output->print(i);
      output->print(" ");
      epochToString(eventTimestamps[i],output);
      output->print(" ");
      output->print(EVENT_LOG_DESCRIPTION[eventTypes[i]]);


      if (eventParameters[i]!=FLOAT_MIN_VALUE) {
        output->print(": ");
        output->print(eventParameters[i]);
      }
      output->println("");
    } 
  }
}









