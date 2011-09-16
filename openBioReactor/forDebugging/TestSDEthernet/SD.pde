



Sd2Card card;
SdVolume volume;

void setupSD() {
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  // see if the card is present and can be initialized:
  if (!SD.begin(SD_SELECT)) {
    Serial.println("Card failed, or not present");
    return;
  }
  if (DEBUG)  Serial.println("card initialized.");
  
  if (!card.init(SPI_HALF_SPEED, SD_SELECT)) error("card.init failed!");
  if (!volume.init(&card)) error("vol.init failed!");

    
    
}





