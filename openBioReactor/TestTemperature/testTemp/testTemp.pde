#include <OneWire.h>

boolean DEBUG = true;

// DS18S20 Temperature chip i/o
OneWire ds(11);  // on pin 11


void setup(void) {
  // initialize inputs/outputs

  // start serial port
  Serial.begin(9600);
  //pinMode(0, OUTPUT);
  //pinMode(2, OUTPUT);
  analogWrite(7, 0);  
  analogWrite(12, 0);

}


void loop(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;

  float tempMeasured = 0;

  if ( !ds.search(addr)) 
  {
    if(DEBUG) Serial.print("No more addresses.\n");
    ds.reset_search();
    if(DEBUG) Serial.print("No device on 1-wire bus.\n");
    return;
  }


  if(DEBUG)
  {
    Serial.print("R=");
    for( i = 0; i < 8; i++) {
      Serial.print(addr[i], HEX);
      Serial.print(" ");
    }
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.print("CRC is not valid!\n");
    if(DEBUG) Serial.print("TEPMERATURE device: wrong CRC.\n");  
    return;
  }

  if(DEBUG)
  {
    if (addr[0] == 0x28) {
      Serial.print("Device DS18B20 has been successfully recognized.\n");
    }
    else {
      Serial.print("Device family is not recognized: 0x");
      Serial.println(addr[0],HEX);
      Serial.print("1-wire device not recognized on bus.\n");      
      return;
    }
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // ...,1); means start conversion, with parasite power on at the end

  delay(1000);     // if NOT on parasitic power mode, waiting is not required; 
                // sensor changes temp every 750ms but is always accessible (tested with every 10ms)
  
  // we might do a ds.depower() here, but the reset will take care of it.
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++)           // we need 9 bytes
  { 
    data[i] = ds.read();
  }

  if(DEBUG)
  { 

    Serial.print("P=");
    Serial.print(present,HEX);
    Serial.print(" ");
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = ds.read();
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.print(" CRC=");
    Serial.print( OneWire::crc8( data, 8), HEX);
    Serial.println();
  }


  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
  Fract = Tc_100 % 100;


  Serial.print("Temperature is:\t");
  if (SignBit) // If its negative
  {
    Serial.print("-");
  }

  tempMeasured = Tc_100;
  tempMeasured /= 100;

  Serial.print(tempMeasured);
  Serial.print(" C \n");



}




