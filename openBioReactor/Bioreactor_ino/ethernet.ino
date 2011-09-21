void ethernetSetup();
void ethernetUpdateNTPTime();
unsigned long ethernetSendNTPpacket(byte *address);
unsigned long ethernetGetTimeStamp();
void ethernetPushLog(String logString);
void ethernetGetCommand();

/*
  Web client
 
 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 18 Dec 2009
 by David A. Mellis
 
 
 Udp NTP Client
 
 Get the time from a Network Time Protocol (NTP) time server
 Demonstrates use of UDP sendPacket and ReceivePacket 
 For more on NTP time servers and the messages needed to communicate with them, 
 see http://en.wikipedia.org/wiki/Network_Time_Protocol
 
 created 4 Sep 2010 
 by Michael Margolis
 modified 17 Sep 2010
 by Tom Igoe
 
 This code is in the public domain.
 
 */


#define MAX_HTTP_STRING_LENGTH 2048    // in bytes; max. http return string to read  
// (reserve space for the http header !AND! the JSON command string)
#define MAX_COMMAND_STRING_LENGTH 400 // in bytes; max. JSON return string to read 



// Enter a MAC address and IP address for the Arduino controller below.
// The IP address is reserved on the routher for this Arduino controller.
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {
  10,0,0,28};


// Parameters for the NTP clock
unsigned int localPort = 8888;      // local port to listen for UDP packets
IPAddress server(10,0,0,29); // local NTP server 
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 
unsigned long epoch; // Unix time (starts on Jan 1 1970)

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

//WORKAROUND: reserve space for the JSON String (MAX_COMMAND_STRING_LENGTH = 400 bytes);
//because there are some errors in the current used library of the "String" in the Arduino IDE 0022
//static String jsonCommand = "";
//static String jsonCommand = "MemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesMemAllocFor400bytesM";


void ethernetSetup()
{
  // the ethernet pin has been initialized in the main setup() from Bioreactor

  // start Ethernet and UDP
  Ethernet.begin(mac,ip);
//  if (Ethernet.begin(mac,ip) == 0)
//  {
//    if(DEBUG)Serial.println("ERROR: Failed to initialize the Ethernet!"); 
//  }
  Udp.begin(localPort);

}


void ethernetUpdateNTPTime()
{
  digitalWrite(PIN_SD_CARD,HIGH); // Disable SD card first (NEVER FORGET THIS!)
  unsigned long duration = millis();
  ethernetSendNTPpacket(server); // send an NTP packet to a time server
  // wait to see if a reply is available
  int i = 0;
  while(!Udp.parsePacket() && i<100)
  {
    i++;
    delay(1); 
  }  
  if ( Udp.parsePacket() ) 
  {  
   // We've received a packet, read the data from it
    Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;  
    if(DEBUG)Serial.print("Seconds since Jan 1 1900 = " );
    if(DEBUG)Serial.println(secsSince1900);               

    // now convert NTP time into everyday time:
    if(DEBUG)Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;     
    // subtract seventy years:
    epoch = secsSince1900 - seventyYears;  
    // print Unix time:
    Serial.println(epoch);                               


    // print the hour, minute and second
    if(DEBUG)
    {
      Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
      Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
      Serial.print(':');  
      if ( ((epoch % 3600) / 60) < 10 ) 
      {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
      }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':'); 
    if ( (epoch % 60) < 10 ) 
     {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
     }
    Serial.println(epoch %60); // print the second
    }


    if(DEBUG)
    {
      duration = millis() - duration; 
      Serial.print("To get the NTP time stamp took: [ms]");
      Serial.println(duration);
    }

  }
}



// send an NTP request to the time server at the given address 
unsigned long ethernetSendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  if(DEBUG)Serial.println("Requesting NTP timestamp from server."); 	   
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
}

unsigned long ethernetGetTimeStamp()
{
  return epoch; 
}


void ethernetPushLog(String logString)
{
  digitalWrite(PIN_SD_CARD,HIGH); // Disable SD card first
  int i=0;

  //open a connection and try to print 
  if(DEBUG)Serial.println("Connecting...");
  while(client.connect(server, 80) && i<500)
  {
    i++;
    delay(1);
  }

  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.connected()) {
    //    char c = client.read();
    //    if(DEBUG)Serial.print(c);
    
    // use strcpy and strcat and work with CHAR arrays instead of "String" from Arduino;
   //because of bugs in String memory allocation!! (Arduino IDE: 0022)
   
     int dataStringLength = logString.length() + 1; // else the last character is not printed
      char bufferUrl[dataStringLength];
      dataStringLength += 50; // reserve space for aditional characters of the URL string
      char pushUrl[dataStringLength];
      logString.toCharArray(bufferUrl,sizeof(bufferUrl));
          Serial.print(sizeof(bufferUrl));
        Serial.println(bufferUrl);
        
      strcpy(pushUrl,"GET /bioReacTor/add.php?");
      strcat(pushUrl,bufferUrl);
      strcat(pushUrl," HTTP/1.0\n\n");
    
              Serial.print(sizeof(pushUrl));
        Serial.println(pushUrl);
        client.println(pushUrl);

    if(DEBUG)Serial.print("The log has been pushed to the server.");

  }

  client.stop();
  if(DEBUG)Serial.println("Disconnected.");

}

void ethernetGetCommand()
{
  digitalWrite(PIN_SD_CARD,HIGH); // Disable SD card first
  int i = 0;
  String jsonCommand = String(""); // reinitialize alocated string
  //char jsonCommandBuffer[MAX_HTTP_STRING_LENGTH]; // reserve space
//  if(DEBUG)Serial.print("String initialized has length of: ");
//  if(DEBUG)Serial.println(sizeof(jsonCommand));
  //open a connection and try to read the JSON command 
  if(DEBUG)Serial.println("Connecting to get newcommand...");
  while(client.connect(server, 80) && i<500) // 500ms timeout
  {
    i++;
    delay(1);
  }

  // read the JSON object from the server and save it in a local String
  // for more informations look up www.json.org
  int breakController = 0;
  int jsonController = 0;
  client.println("GET /bioReacTor/command HTTP/1.0\n"); //connect to the command page of the server
  delay(100); // small delay is neccessary and 100ms has turned out to be a sufficient value
  while (client.connected()) 
  {
    if(breakController >= MAX_HTTP_STRING_LENGTH) 
    {
      break; // if the read String to long
      Serial.println("ERROR: The input string on [localserver]/status is too long! Adjust the string length or the http header!");
    }

    breakController++;

    char readChar = client.read();
    if(DEBUG) Serial.print(readChar);
    if(readChar == '{') // start of the JSON command string
    {
      for(readChar; readChar!='}'; readChar = client.read())// until end of JSON command string reached
      {
        //if(DEBUG)Serial.print(readChar);
        if(jsonController > MAX_COMMAND_STRING_LENGTH)
        {  
          Serial.println("ERROR: The input string on [localserver]/status is too long! Adjust the max. string length or the JSON string!");
          break;
        }
        jsonCommand += readChar;
        jsonController++;      
      }
      jsonCommand += "}"; // complete the JSON string 
      break; // get out of while(client.connected()) and stop connection
    }

  }

 
  client.stop();
  if(DEBUG)Serial.println("Disconnected from getting commands.");
  

  if(DEBUG)Serial.print("The json command string length is: [] ");
if(DEBUG)Serial.print(jsonCommand.length());
  if(DEBUG)Serial.println(" and the string is: ");
if(DEBUG)Serial.println(jsonCommand);



  //---------Evaluate the JSON command string---------
  if(jsonCommand.equals(""))
  {
    Serial.println("ERROR: The JSON command string is empty! Please check the webserver's command site!");
  }
  else
  {
    String searchString = "";
    int indexBufferStart;
    int indexBufferEnd;
    String extractedValue = "";
    char extractedValueChar[10]; // 10 characters for the buffer is enough
    float extractedValueFloat;
    int extractedValueInt;
    
    //---------Set Temperature----------

    searchString = "liquidTemp:"; // don't forget the separator ':'
    indexBufferStart = jsonCommand.indexOf(searchString); // index of the first letter
    indexBufferEnd = jsonCommand.indexOf(",",indexBufferStart); // search the end-index of the name + value in the JSON object
    if(indexBufferEnd > indexBufferStart)// continue
    {
      indexBufferStart += searchString.length();

      extractedValue = jsonCommand.substring(indexBufferStart,indexBufferEnd);//extract the temperature
      extractedValue.toCharArray(extractedValueChar,10); // 10 characters for the buffer is enough
      extractedValueFloat = atof(extractedValueChar);

      // first check if there is a difference between the read value and the stored one
      // (because floats are used, an error value of 0.05 has been set!)
      if(HEATING_TEMPERATURE_LIMIT >= extractedValueFloat + 0.05 || HEATING_TEMPERATURE_LIMIT <= extractedValueFloat-0.05)
      {
        // check if the input value is valid, then safe it
        if(extractedValueFloat > HEAGINT_MAX_ALLOWED_LIMIT)
        {
          if(DEBUG)Serial.println("WARNING: The input temperature is to high! New temperature value has not been set.");
        }
        else if (extractedValueFloat < HEAGINT_MIN_ALLOWED_LIMIT)
        {
          if(DEBUG)Serial.println("WARNING: The input temperature is to low! New temperature value has not been set.");
        }
        else
        {
          HEATING_TEMPERATURE_LIMIT = extractedValueFloat;
          if(DEBUG)Serial.print("The new temperature has been successfully set to: [C]");
          if(DEBUG)Serial.println(HEATING_TEMPERATURE_LIMIT);
        }
      }
      else if(DEBUG)Serial.println("The set temperature is the same as the saved one (error: 0.05).");



//      if(DEBUG)Serial.print("The indexes are: ");
//      if(DEBUG)Serial.print(indexBufferStart);
//      if(DEBUG)Serial.print(" and ");
//      if(DEBUG)Serial.println(indexBufferEnd);  
//      if(DEBUG)Serial.print("The extractedValue length is: ");
//      if(DEBUG)Serial.println(extractedValue.length());   
//      if(DEBUG)Serial.print("The extracted string is: ");
//      if(DEBUG)Serial.println(extractedValue);    
//      if(DEBUG)Serial.print("The extracted char is: ");
//      if(DEBUG)Serial.println(extractedValueChar);
//      if(DEBUG)Serial.print("The extracted float is: ");
//      if(DEBUG)Serial.println(extractedValueFloat);
    }
    else Serial.println("ERROR: The term 'liquidTemp:' couldn't have been found in the JSON command string. Please verify the command string.");



    //---------Set liquid level----------

    searchString = "liquidLevel:"; // don't forget the separator ':'
    indexBufferStart = jsonCommand.indexOf(searchString); // index of the first letter
    indexBufferEnd = jsonCommand.indexOf(",",indexBufferStart); // search the end-index of the name + value in the JSON object
    if(indexBufferEnd > indexBufferStart)// continue
    {
      indexBufferStart += searchString.length();

      extractedValue = jsonCommand.substring(indexBufferStart,indexBufferEnd);//extract the temperature
      extractedValue.toCharArray(extractedValueChar,6); // 6 characters for the buffer is enough
      extractedValueFloat = atof(extractedValueChar);

      // first check if there is a difference between the read value and the stored one
      // (because floats are used, an error value of 0.05 has been set!)
      if(LIQUID_LEVEL_SET >= extractedValueFloat+0.05 || LIQUID_LEVEL_SET <= extractedValueFloat-0.05)
      {
        // check if the input value is valid, then safe it
        if(extractedValueFloat > LIQUID_LEVEL_MAX)
        {
          if(DEBUG)Serial.println("WARNING: The input liquid level is to high! New liquid value has not been set.");
        }
        else if (extractedValueFloat < 0.0)
        {
          if(DEBUG)Serial.println("WARNING: The input liquid level negative! New liquid value has not been set.");
        }
        else
        {
          LIQUID_LEVEL_SET = extractedValueFloat;
          if(DEBUG)Serial.print("The new liquid level has been successfully set to: [inch]");
          if(DEBUG)Serial.println(LIQUID_LEVEL_SET);
        }
      }
      else if(DEBUG)Serial.println("The set liquid level is the same as the saved one (error: 0.05).");


//
//      if(DEBUG)Serial.print("The indexes are: ");
//      if(DEBUG)Serial.print(indexBufferStart);
//      if(DEBUG)Serial.print(" and ");
//      if(DEBUG)Serial.println(indexBufferEnd);  
//      if(DEBUG)Serial.print("The extractedValue length is: ");
//      if(DEBUG)Serial.println(extractedValue.length());   
//      if(DEBUG)Serial.print("The extracted string is: ");
//      if(DEBUG)Serial.println(extractedValue);    
//      if(DEBUG)Serial.print("The extracted char is: ");
//      if(DEBUG)Serial.println(extractedValueChar);
//      if(DEBUG)Serial.print("The extracted float is: ");
//      if(DEBUG)Serial.println(extractedValueFloat);

    }
    else Serial.println("ERROR: The term 'liquidLevel:' couldn't have been found in the JSON command string. Please verify the command string.");


//---------Set pH level----------

    searchString = "pH:"; // don't forget the separator ':'
    indexBufferStart = jsonCommand.indexOf(searchString); // index of the first letter
    indexBufferEnd = jsonCommand.indexOf(",",indexBufferStart); // search the end-index of the name + value in the JSON object
    
    if(indexBufferEnd > indexBufferStart)// continue
    {
      indexBufferStart += searchString.length();

      extractedValue = jsonCommand.substring(indexBufferStart,indexBufferEnd);//extract the temperature
      extractedValue.toCharArray(extractedValueChar,6); // 6 characters for the buffer is enough
      extractedValueFloat = atof(extractedValueChar);

      // first check if there is a difference between the read value and the stored one
      // (because floats are used, an error value of 0.05 has been set!)
      if(pH_SET >= extractedValueFloat+0.05 || pH_SET <= extractedValueFloat-0.05)
      {
        // check if the input value is valid, then safe it
        if(extractedValueFloat > 14.0) // max pH level is 14
        {
          if(DEBUG)Serial.println("WARNING: The input pH level is to high! New pH value has not been set.");
        }
        else if (extractedValueFloat < 0.0) // min pH level is 0
        {
          if(DEBUG)Serial.println("WARNING: The input pH level negative! New pH value has not been set.");
        }
        else
        {
          pH_SET = extractedValueFloat;
          if(DEBUG)Serial.print("The new pH level has been successfully set to: ");
          if(DEBUG)Serial.println(pH_SET);
        }
      }
      else if(DEBUG)Serial.println("The set temperature is the same as the saved one (error: 0.05).");



//      if(DEBUG)Serial.print("The indexes are: ");
//      if(DEBUG)Serial.print(indexBufferStart);
//      if(DEBUG)Serial.print(" and ");
//      if(DEBUG)Serial.println(indexBufferEnd);  
//      if(DEBUG)Serial.print("The extractedValue length is: ");
//      if(DEBUG)Serial.println(extractedValue.length());   
//      if(DEBUG)Serial.print("The extracted string is: ");
//      if(DEBUG)Serial.println(extractedValue);    
//      if(DEBUG)Serial.print("The extracted char is: ");
//      if(DEBUG)Serial.println(extractedValueChar);
//      if(DEBUG)Serial.print("The extracted float is: ");
//      if(DEBUG)Serial.println(extractedValueFloat);

    }
    else Serial.println("ERROR: The term 'pH:' couldn't have been found in the JSON command string. Please verify the command string.");


//---------Set Bioreactor state ----------

    searchString = "mode:"; // don't forget the separator ':'
    indexBufferStart = jsonCommand.indexOf(searchString); // index of the first letter
    indexBufferEnd = jsonCommand.indexOf(",",indexBufferStart); // WORKAROUND: pH is the last command string; therefore '}'; else change to','
    if(indexBufferEnd > indexBufferStart)// continue
    {
      indexBufferStart += searchString.length();

      extractedValue = jsonCommand.substring(indexBufferStart,indexBufferEnd);//extract the temperature
      extractedValue.toCharArray(extractedValueChar,6); // 6 characters for the buffer is enough
      extractedValueInt = atoi(extractedValueChar);

      // first check if there is a difference between the read value and the stored one
        // then check if in pumping mode; ONLY change from pumping mode if MANUAL mode is selected
      if(extractedValueInt != BIOREACTOR_MODE
         && (BIOREACTOR_MODE != BIOREACTOR_PUMPING_MODE 
         || (BIOREACTOR_MODE == BIOREACTOR_PUMPING_MODE && extractedValueInt == BIOREACTOR_MANUAL_MODE)))
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == BIOREACTOR_STANDBY_MODE
          || extractedValueInt == BIOREACTOR_RUNNING_MODE
          || extractedValueInt == BIOREACTOR_MANUAL_MODE)
        {

          //switch Bioreactor mode
          BIOREACTOR_MODE = extractedValueInt;
          if(DEBUG)Serial.print("The Bioreactor has been set to a new state: ");
          if(DEBUG)Serial.println(BIOREACTOR_MODE);
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: The Bioreactor state is invalid!.");
        }
      }
      else if(DEBUG)Serial.println("The Bioreactor state is the same as the saved one.");

      if(DEBUG)Serial.print("The indexes are: ");
      if(DEBUG)Serial.print(indexBufferStart);
      if(DEBUG)Serial.print(" and ");
      if(DEBUG)Serial.println(indexBufferEnd);  
      if(DEBUG)Serial.print("The extractedValue length is: ");
      if(DEBUG)Serial.println(extractedValue.length());   
      if(DEBUG)Serial.print("The extracted string is: ");
      if(DEBUG)Serial.println(extractedValue);    
      if(DEBUG)Serial.print("The extracted char is: ");
      if(DEBUG)Serial.println(extractedValueChar);
      if(DEBUG)Serial.print("The extracted int is: ");
      if(DEBUG)Serial.println(extractedValueInt);

    }
    else Serial.println("ERROR: The term 'mode:' couldn't have been found in the JSON command string. Please verify the command string.");
  
    //---------Set pumpOut state ----------
  // only get the pump's state if in MANUAL mode
  if(BIOREACTOR_MODE == BIOREACTOR_MANUAL_MODE)
  {
    searchString = "pumpOut:"; // don't forget the separator ':'
    indexBufferStart = jsonCommand.indexOf(searchString); // index of the first letter
    indexBufferEnd = jsonCommand.indexOf(",",indexBufferStart); // search the end-index of the name + value in the JSON object
    if(indexBufferEnd > indexBufferStart)// continue
    {
      indexBufferStart += searchString.length();

      extractedValue = jsonCommand.substring(indexBufferStart,indexBufferEnd);//extract the temperature
      extractedValue.toCharArray(extractedValueChar,6); // 6 characters for the buffer is enough
      extractedValueInt = atoi(extractedValueChar);

      // first check if there is a difference between the read value and the stored one
      if(extractedValueInt != relaySwitchPumpOutGetState())
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == 1 || extractedValueInt == 0 )
        {
          //turn ON or OFF
          if(extractedValueInt == 1) relaySwitchPumpOutTurnOn();
          else relaySwitchPumpOutTurnOff(); // if extractedValueInt = 0
          if(DEBUG)Serial.println("PumpOut has been set to a new state.");
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: PumpOut state is invalid!.");
        }
      }
      else if(DEBUG)Serial.println("PumpOut state is the same as the saved one.");

//      if(DEBUG)Serial.print("The indexes are: ");
//      if(DEBUG)Serial.print(indexBufferStart);
//      if(DEBUG)Serial.print(" and ");
//      if(DEBUG)Serial.println(indexBufferEnd);  
//      if(DEBUG)Serial.print("The extractedValue length is: ");
//      if(DEBUG)Serial.println(extractedValue.length());   
//      if(DEBUG)Serial.print("The extracted string is: ");
//      if(DEBUG)Serial.println(extractedValue);    
//      if(DEBUG)Serial.print("The extracted char is: ");
//      if(DEBUG)Serial.println(extractedValueChar);
//      if(DEBUG)Serial.print("The extracted int is: ");
//      if(DEBUG)Serial.println(extractedValueInt);

    }
    else Serial.println("ERROR: The term 'pumpOut:' couldn't have been found in the JSON command string. Please verify the command string.");

  }
  else if(DEBUG) Serial.println("Not in MANUAL mode: PumpOut's value not taken");
  
//---------Set pumpIn state ----------
  // only get the pump's state if in MANUAL mode
  if(BIOREACTOR_MODE == BIOREACTOR_MANUAL_MODE)
  {
    searchString = "pumpIn:"; // don't forget the separator ':'
    indexBufferStart = jsonCommand.indexOf(searchString); // index of the first letter
    indexBufferEnd = jsonCommand.indexOf(",",indexBufferStart); // search the end-index of the name + value in the JSON object
    if(indexBufferEnd > indexBufferStart)// continue
    {
      indexBufferStart += searchString.length();

      extractedValue = jsonCommand.substring(indexBufferStart,indexBufferEnd);//extract the temperature
      extractedValue.toCharArray(extractedValueChar,6); // 6 characters for the buffer is enough
      extractedValueInt = atoi(extractedValueChar);

      // first check if there is a difference between the read value and the stored one
      if(extractedValueInt != relaySwitchPumpInGetState())
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == 1 || extractedValueInt == 0 )
        {
          //turn ON or OFF
          if(extractedValueInt == 1) relaySwitchPumpInTurnOn();
          else relaySwitchPumpInTurnOff(); // if extractedValueInt = 0
          if(DEBUG)Serial.println("PumpIn has been set to a new state.");
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: PumpIn state is invalid!.");
        }
      }
      else if(DEBUG)Serial.println("PumpIn state is the same as the saved one.");

//      if(DEBUG)Serial.print("The indexes are: ");
//      if(DEBUG)Serial.print(indexBufferStart);
//      if(DEBUG)Serial.print(" and ");
//      if(DEBUG)Serial.println(indexBufferEnd);  
//      if(DEBUG)Serial.print("The extractedValue length is: ");
//      if(DEBUG)Serial.println(extractedValue.length());   
//      if(DEBUG)Serial.print("The extracted string is: ");
//      if(DEBUG)Serial.println(extractedValue);    
//      if(DEBUG)Serial.print("The extracted char is: ");
//      if(DEBUG)Serial.println(extractedValueChar);
//      if(DEBUG)Serial.print("The extracted int is: ");
//      if(DEBUG)Serial.println(extractedValueInt);

    }
    else Serial.println("ERROR: The term 'pumpIn:' couldn't have been found in the JSON command string. Please verify the command string.");
  
  }
  else if(DEBUG) Serial.println("Not in MANUAL mode: PumpIn's value not taken");

    //---------Set motor state ----------
// only get the motor's state if in MANUAL mode
  if(BIOREACTOR_MODE == BIOREACTOR_MANUAL_MODE)
  {
    searchString = "motor:"; // don't forget the separator ':'
    indexBufferStart = jsonCommand.indexOf(searchString); // index of the first letter
    indexBufferEnd = jsonCommand.indexOf(",",indexBufferStart); // search the end-index of the name + value in the JSON object
    if(indexBufferEnd > indexBufferStart)// continue
    {
      indexBufferStart += searchString.length();

      extractedValue = jsonCommand.substring(indexBufferStart,indexBufferEnd);//extract the temperature
      extractedValue.toCharArray(extractedValueChar,6); // 6 characters for the buffer is enough
      extractedValueInt = atoi(extractedValueChar);

      // first check if there is a difference between the read value and the stored one
      if(extractedValueInt != relaySwitchMotorGetState())
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == 1 || extractedValueInt == 0 )
        {
          //turn ON or OFF
          if(extractedValueInt == 1) relaySwitchMotorTurnOn();
          else relaySwitchMotorTurnOff(); // if extractedValueInt = 0
          if(DEBUG)Serial.println("The motor has been set to a new state.");
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: The motor state is invalid!.");
        }
      }
      else if(DEBUG)Serial.println("The motor state is the same as the saved one.");

//      if(DEBUG)Serial.print("The indexes are: ");
//      if(DEBUG)Serial.print(indexBufferStart);
//      if(DEBUG)Serial.print(" and ");
//      if(DEBUG)Serial.println(indexBufferEnd);  
//      if(DEBUG)Serial.print("The extractedValue length is: ");
//      if(DEBUG)Serial.println(extractedValue.length());   
//      if(DEBUG)Serial.print("The extracted string is: ");
//      if(DEBUG)Serial.println(extractedValue);    
//      if(DEBUG)Serial.print("The extracted char is: ");
//      if(DEBUG)Serial.println(extractedValueChar);
//      if(DEBUG)Serial.print("The extracted int is: ");
//      if(DEBUG)Serial.println(extractedValueInt);

    }
    else Serial.println("ERROR: The term 'motor:' couldn't have been found in the JSON command string. Please verify the command string.");

  }
  else if(DEBUG) Serial.println("Not in MANUAL mode: Motor's value not taken");


//---------Set gas sensor (methane) state ----------
  // only get the pump's state if in MANUAL mode
  if(BIOREACTOR_MODE == BIOREACTOR_MANUAL_MODE)
  {
    searchString = "methane:"; // don't forget the separator ':'
    indexBufferStart = jsonCommand.indexOf(searchString); // index of the first letter
    indexBufferEnd = jsonCommand.indexOf(",",indexBufferStart); // search the end-index of the name + value in the JSON object
    if(indexBufferEnd > indexBufferStart)// continue
    {
      indexBufferStart += searchString.length();

      extractedValue = jsonCommand.substring(indexBufferStart,indexBufferEnd);//extract the temperature
      extractedValue.toCharArray(extractedValueChar,6); // 6 characters for the buffer is enough
      extractedValueInt = atoi(extractedValueChar);

      // first check if there is a difference between the read value and the stored one
      if(extractedValueInt != gasValvesCH4GetState())
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == 1 || extractedValueInt == 0 )
        {
          //turn ON or OFF
          if(extractedValueInt == 1) gasValvesCH4TurnOn();
          else gasValvesCH4TurnOff(); // if extractedValueInt = 0
          if(DEBUG)Serial.println("The gas sensor (methane) has been set to a new state.");
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: The gas sensor (methane) state is invalid!.");
        }
      }
      else if(DEBUG)Serial.println("The gas sensor (methane) state is the same as the saved one.");

//      if(DEBUG)Serial.print("The indexes are: ");
//      if(DEBUG)Serial.print(indexBufferStart);
//      if(DEBUG)Serial.print(" and ");
//      if(DEBUG)Serial.println(indexBufferEnd);  
//      if(DEBUG)Serial.print("The extractedValue length is: ");
//      if(DEBUG)Serial.println(extractedValue.length());   
//      if(DEBUG)Serial.print("The extracted string is: ");
//      if(DEBUG)Serial.println(extractedValue);    
//      if(DEBUG)Serial.print("The extracted char is: ");
//      if(DEBUG)Serial.println(extractedValueChar);
//      if(DEBUG)Serial.print("The extracted int is: ");
//      if(DEBUG)Serial.println(extractedValueInt);

    }
    else Serial.println("ERROR: The term 'methane:' couldn't have been found in the JSON command string. Please verify the command string.");
  }
  else if(DEBUG) Serial.println("Not in MANUAL mode: Gas valve's value not taken");

 //---------Set gas sensor (carbonDioxide) state ----------
  // only get the pump's state if in MANUAL mode
  if(BIOREACTOR_MODE == BIOREACTOR_MANUAL_MODE)
  {
    searchString = "carbonDioxide:"; // don't forget the separator ':'
    indexBufferStart = jsonCommand.indexOf(searchString); // index of the first letter
    indexBufferEnd = jsonCommand.indexOf(",",indexBufferStart); // search the end-index of the name + value in the JSON object
    if(indexBufferEnd > indexBufferStart)// continue
    {
      indexBufferStart += searchString.length();

      extractedValue = jsonCommand.substring(indexBufferStart,indexBufferEnd);//extract the temperature
      extractedValue.toCharArray(extractedValueChar,6); // 6 characters for the buffer is enough
      extractedValueInt = atoi(extractedValueChar);

      // first check if there is a difference between the read value and the stored one
      if(extractedValueInt != gasValvesCO2GetState())
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == 1 || extractedValueInt == 0 )
        {
          //turn ON or OFF
          if(extractedValueInt == 1) gasValvesCO2TurnOn();
          else gasValvesCO2TurnOff(); // if extractedValueInt = 0
          if(DEBUG)Serial.println("The gas sensor (carbonDioxide) has been set to a new state.");
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: The gas sensor (carbonDioxide) state is invalid!.");
        }
      }
      else if(DEBUG)Serial.println("The gas sensor (carbonDioxide) state is the same as the saved one.");

//      if(DEBUG)Serial.print("The indexes are: ");
//      if(DEBUG)Serial.print(indexBufferStart);
//      if(DEBUG)Serial.print(" and ");
//      if(DEBUG)Serial.println(indexBufferEnd);  
//      if(DEBUG)Serial.print("The extractedValue length is: ");
//      if(DEBUG)Serial.println(extractedValue.length());   
//      if(DEBUG)Serial.print("The extracted string is: ");
//      if(DEBUG)Serial.println(extractedValue);    
//      if(DEBUG)Serial.print("The extracted char is: ");
//      if(DEBUG)Serial.println(extractedValueChar);
//      if(DEBUG)Serial.print("The extracted int is: ");
//      if(DEBUG)Serial.println(extractedValueInt);

    }
    else Serial.println("ERROR: The term 'carbonDioxide:' couldn't have been found in the JSON command string. Please verify the command string.");
  }
  else if(DEBUG) Serial.println("Not in MANUAL mode: Gas valve's value not taken");


//---------Set gas sensor (nitrogen) state ----------
  // only get the pump's state if in MANUAL mode
  if(BIOREACTOR_MODE == BIOREACTOR_MANUAL_MODE)
  {
    searchString = "nitrogen:"; // don't forget the separator ':'
    indexBufferStart = jsonCommand.indexOf(searchString); // index of the first letter
    indexBufferEnd = jsonCommand.indexOf("}",indexBufferStart); 
  // THE '}' IS ONLY A WORKAROUND; change to ',' if not at the end of the string
    if(indexBufferEnd > indexBufferStart)// continue
    {
      indexBufferStart += searchString.length();

      extractedValue = jsonCommand.substring(indexBufferStart,indexBufferEnd);//extract the temperature
      extractedValue.toCharArray(extractedValueChar,6); // 6 characters for the buffer is enough
      extractedValueInt = atoi(extractedValueChar);

      // first check if there is a difference between the read value and the stored one
      if(extractedValueInt != gasValvesN2GetState())
      {
        // check if the input value is valid, then safe it
        if(extractedValueInt == 1 || extractedValueInt == 0 )
        {
          //turn ON or OFF
          if(extractedValueInt == 1) gasValvesN2TurnOn();
          else gasValvesN2TurnOff(); // if extractedValueInt = 0
          if(DEBUG)Serial.println("The gas sensor (nitrogen) has been set to a new state.");
        }
        else
        {
          if(DEBUG)Serial.println("WARNING: The gas sensor (nitrogen) state is invalid!.");
        }
      }
      else if(DEBUG)Serial.println("The gas sensor (nitrogen) state is the same as the saved one.");

//      if(DEBUG)Serial.print("The indexes are: ");
//      if(DEBUG)Serial.print(indexBufferStart);
//      if(DEBUG)Serial.print(" and ");
//      if(DEBUG)Serial.println(indexBufferEnd);  
//      if(DEBUG)Serial.print("The extractedValue length is: ");
//      if(DEBUG)Serial.println(extractedValue.length());   
//      if(DEBUG)Serial.print("The extracted string is: ");
//      if(DEBUG)Serial.println(extractedValue);    
//      if(DEBUG)Serial.print("The extracted char is: ");
//      if(DEBUG)Serial.println(extractedValueChar);
//      if(DEBUG)Serial.print("The extracted int is: ");
//      if(DEBUG)Serial.println(extractedValueInt);

    }
    else Serial.println("ERROR: The term 'nitrogen:' couldn't have been found in the JSON command string. Please verify the command string.");
  }
  else if(DEBUG) Serial.println("Not in MANUAL mode: Gas valve's value not taken");

 
  
  }// end of if(jsonCommand.equals(""))

}





