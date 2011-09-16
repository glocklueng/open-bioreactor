unsigned static int LOCAL_UDP_PORT=8888;      // local port to listen for UDP packets


// Should be configurable and stored in EEPROM
byte static NTP_SERVER[] = { 
  192, 43, 244, 18}; // time.nist.gov NTP server

const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 


void setupNtpTime() {
  Udp.begin(LOCAL_UDP_PORT);
  // we will try to update the time every hour
  timer.setInterval(3600000, updateTime);
}

void updateTime() {
  updateTime(false); 
}

/*
Update the time of the microcontroller.
If sync is true we wait to get the answer, otherwise it is just one more task to do
*/
void updateTime(boolean sync)
{
  sendNTPpacket(NTP_SERVER);
  if (sync) {
    delay(1000);
    setNtpTime();
  } 
  else {
    // getting the UDP packet takes some time and we don't want to block everything
    // we will just check after 1.5s
    timer.setTimeout(1500, setNtpTime);
  }
}

void setNtpTime() {
  if ( Udp.available() ) {
    Udp.readPacket(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:
    unsigned long secsSince1900 =  ((long)word(packetBuffer[40], packetBuffer[41])) << 16 | ((long)word(packetBuffer[42], packetBuffer[43]));  
    if (DEBUG) {
      Serial.print("Seconds since Jan 1 1900 = " );
      Serial.println(secsSince1900);       
    }

    const unsigned long seventy_years = 2208988800UL;

    setTime(secsSince1900 -  seventy_years);

    if (DEBUG) {
      Serial.print("Time was set to epoch: ");
      Serial.println(secsSince1900 -  seventy_years);
    }
  } 
  else {
    if (DEBUG) {
      Serial.println("NTP server not accessible");
    }
    addEvent(NTP_SERVER_NOT_FOUND);
  }

}



// send an NTP request to the time server at the given address 
unsigned long sendNTPpacket(byte *address)
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
  Udp.sendPacket( packetBuffer,NTP_PACKET_SIZE,  address, 123); //NTP requests are to port 123
}







