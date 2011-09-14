

void sdCardSetup()
{
  // the sdCard pin has been initialized in the main setup() from Bioreactor
  
  // start Ethernet and UDP
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);

}
