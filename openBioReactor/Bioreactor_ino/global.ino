void globalFloatToStringDouble(double val, byte precision, char *buf, unsigned bufLen = 0xffff);
unsigned globalFloatToStringUnsigned(unsigned long val, char *buf, unsigned bufLen = 0xffff, byte width = 0);
void globalUpdateSensors();
//boolean globalSelectSDCard();
//boolean globalSelectEthernet();

//
// Produce a formatted string in a buffer corresponding to the value provided.
// If the 'width' parameter is non-zero, the value will be padded with leading
// zeroes to achieve the specified width.  The number of characters added to
// the buffer (not including the null termination) is returned.
//
unsigned
globalFloatToStringUnsigned(unsigned long val, char *buf, unsigned bufLen, byte width)
{
  if (!buf || !bufLen)
    return(0);

  // produce the digit string (backwards in the digit buffer)
  char dbuf[10];
  unsigned idx = 0;
  while (idx < sizeof(dbuf))
  {
    dbuf[idx++] = (val % 10) + '0';
    if ((val /= 10) == 0)
      break;
  }

  // copy the optional leading zeroes and digits to the target buffer
  unsigned len = 0;
  byte padding = (width > idx) ? width - idx : 0;
  char c = '0';
  while ((--bufLen > 0) && (idx || padding))
  {
    if (padding)
      padding--;
    else
      c = dbuf[--idx];
    *buf++ = c;
    len++;
  }

  // add the null termination
  *buf = '\0';
  return(len);
}

//
// Format a floating point value with number of decimal places.
// The 'precision' parameter is a number from 0 to 6 indicating the desired decimal places.
// The 'buf' parameter points to a buffer to receive the formatted string.  This must be
// sufficiently large to contain the resulting string.  The buffer's length may be
// optionally specified.  If it is given, the maximum length of the generated string
// will be one less than the specified value.
//
// example: globalFloatToStringDouble(3.1415, 2, buf); // produces 3.14 (two decimal places)
//
void globalFloatToStringDouble(double val, byte precision, char *buf, unsigned bufLen)
{
  if (!buf || !bufLen)
    return;

  // limit the precision to the maximum allowed value
  const byte maxPrecision = 6;
  if (precision > maxPrecision)
    precision = maxPrecision;

  if (--bufLen > 0)
  {
    // check for a negative value
    if (val < 0.0)
    {
      val = -val;
      *buf = '-';
      bufLen--;
    }

    // compute the rounding factor and fractional multiplier
    double roundingFactor = 0.5;
    unsigned long mult = 1;
    for (byte i = 0; i < precision; i++)
    {
      roundingFactor /= 10.0;
      mult *= 10;
    }

    if (bufLen > 0)
    {
      // apply the rounding factor
      val += roundingFactor;

      // add the integral portion to the buffer
      unsigned len = globalFloatToStringUnsigned((unsigned long)val, buf, bufLen);
      buf += len;
      bufLen -= len;
    }

    // handle the fractional portion
    if ((precision > 0) && (bufLen > 0))
    {
      *buf++ = '.';
      if (--bufLen > 0)
        buf += globalFloatToStringUnsigned((unsigned long)((val - (unsigned long)val) * mult), buf, bufLen, precision);
    }
  }

  // null-terminate the string
  *buf = '\0';
} 

void globalUpdateSensors()
{
  //temperatureUpdate();
  pHmeterUpdate();
  liquidLevelUpdate();  
}

void globalGetCommandAndPushLog()
{
  //the network related functions have to be together; 
  //else there is a memory allocation failure of the "String"s (bug in Arduino IDE 0022)
   ethernetGetCommand();
   loggingEvent();  
}



//boolean globalSelectSDCard()
//{
//  if(arduinoETHShieldMode != MODE_SDCARD)
//  {
//  //disable the Ethernet pin and (re-)initialize the card.
//    if(DEBUG) Serial.print("\n\nInitializing SD card...");
//    // see if the card is present and can be initialized:
//    digitalWrite(PIN_ETHERNET, HIGH); // deselect Ethernet (MUST be HIGH); PIN_SD_CARD can have any value
//    if (!SD.begin(PIN_SD_CARD)) 
//    {
//      if(DEBUG) Serial.println("ERROR: SD Card failed to initialize, or not present!");
//      return 0;
//    }
//    else 
//    {
//      if(DEBUG )Serial.println("SD card initialized correctly.");
//      arduinoETHShieldMode = MODE_SDCARD;
//      return 1;
//    }
//  }
//  else if(DEBUG)Serial.println("SD card already initialized!");
//
//}


//boolean globalSelectEthernet()
//{
//  if(arduinoETHShieldMode != MODE_ETHERNET)
//  {
//  //disable the SD card pin and (re-)initialize
//    if(DEBUG) Serial.print("\n\nInitializing Ethernet...");
//    digitalWrite(PIN_SD_CARD, HIGH); // deselect SD card (MUST be HIGH);
//    if(DEBUG )Serial.println("Ethernet initialized correctly.");
//    arduinoETHShieldMode = MODE_ETHERNET;
//    return 1;
//    
//  }
//  else if(DEBUG)Serial.println("Ethernet already initialized!");
//
//}


