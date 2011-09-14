#include <OneWire.h>

// DS18S20 Temperature chip i/o
OneWire ds(11);  // on pin ds(XX)
OneWire ds2(9);
int pinHeatingResist = 12;
int pinTest = 13;
float tempMeasured = 0;


void setup(void) {
  // initialize inputs/outputs
  // start serial port
  Serial.begin(9600);
}

void loop(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  byte addr2[8];  
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;

  


  
//  //---------BLINK---------
//  
//  analogWrite(pinTest, 255);   // set the LED on
//  delay(500);              // wait
//  analogWrite(pinTest, 0);    // set the LED off
//  delay(200);              // wait
//  analogWrite(pinTest, 255);   // set the LED on
//  delay(100);              // wait
//  analogWrite(pinTest, 0);    // set the LED off
//  delay(100);              // wait
//  analogWrite(pinTest, 255);   // set the LED on
//  delay(100);              // wait  
//  analogWrite(pinTest, 0);    // set the LED off
//  delay(200);              // wait
  
  
  //---------READ TEMPERATURE
 
   if ( !ds.search(addr)) {
      Serial.print("No more addresses.\n");
      ds.reset_search();
      return;
  }

   if ( !ds2.search(addr2)) {
      Serial.print("No more addresses_2.\n");
      ds.reset_search();
      return;
  }

  Serial.print("R=");
  for( i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
      return;
  }

  if ( addr[0] == 0x10) {
      Serial.print("Device is a DS18S20 family device.\n");
  }
  else if ( addr[0] == 0x28) {
      Serial.print("Device is a DS18B20 family device.\n");
  }
  else {
      Serial.print("Device family is not recognized: 0x");
      Serial.println(addr[0],HEX);
      return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

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


  if (SignBit) // If its negative
  {
     Serial.print("-");
  }
  Serial.print(Whole);
  Serial.print(".");
  if (Fract < 10)
  {
     Serial.print("0");
  }
  Serial.print(Fract);

  Serial.print("\n");
  
  
    Serial.print("Temperature is:\t");
  if (SignBit) // If its negative
  {
    Serial.print("-");
  }

  tempMeasured = Tc_100;
  tempMeasured /= 100;

  Serial.print(tempMeasured);
  Serial.print(" C \n");
  
  //-----------------2. sensor---------------------------
  
    Serial.print("R=");
  for( i = 0; i < 8; i++) {
    Serial.print(addr2[i], HEX);
    Serial.print(" ");
  }

  if ( OneWire::crc8( addr2, 7) != addr2[7]) {
      Serial.print("CRC is not valid!\n");
      return;
  }

  if ( addr2[0] == 0x10) {
      Serial.print("Device is a DS18S20 family device.\n");
  }
  else if ( addr2[0] == 0x28) {
      Serial.print("Device is a DS18B20 family device.\n");
  }
  else {
      Serial.print("Device family is not recognized: 0x");
      Serial.println(addr2[0],HEX);
      return;
  }

  ds2.reset();
  ds2.select(addr2);
  ds2.write(0x44,1);         // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds2.depower() here, but the reset will take care of it.

  present = ds2.reset();
  ds2.select(addr2);    
  ds2.write(0xBE);         // Read Scratchpad

  Serial.print("P=");
  Serial.print(present,HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds2.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print( OneWire::crc8( data, 8), HEX);
  Serial.println();



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


  if (SignBit) // If its negative
  {
     Serial.print("-");
  }
  Serial.print(Whole);
  Serial.print(".");
  if (Fract < 10)
  {
     Serial.print("0");
  }
  Serial.print(Fract);

  Serial.print("\n");
  
  
    Serial.print("Temperature is:\t");
  if (SignBit) // If its negative
  {
    Serial.print("-");
  }

  tempMeasured = Tc_100;
  tempMeasured /= 100;

  Serial.print(tempMeasured);
  Serial.print(" C \n");
  
  //------------HEATING--------------
  //define upper limit and turn off a pin (destined to switch the heating resistance)

  if(tempMeasured <=33) // stop the LED
  {
    analogWrite(pinHeatingResist, 255); 
    Serial.print("Heating...\n");
    
  }
  else
  {
    analogWrite(pinHeatingResist, 0);
    Serial.print("ATTENTION: The temperature has reached given limit!\n");  
  }
  
  
  
}
