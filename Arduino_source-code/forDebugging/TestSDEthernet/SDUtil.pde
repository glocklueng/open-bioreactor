
void sendSDStatusToClient(Client client) {
  httpHeader(client, 200,"text/html");

  client.println("<pre>");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED,SD_SELECT)) {
    client.println("initialization failed. Things to check:");
    client.println("* Is a card inserted?");
    client.println("* Is your wiring correct?");
    client.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } 
  else {
    client.println("Wiring is correct and a card is present."); 
  }

  // print the type of card
  client.print("\nCard type: ");
  switch(card.type()) {
  case SD_CARD_TYPE_SD1:
    client.println("SD1");
    break;
  case SD_CARD_TYPE_SD2:
    client.println("SD2");
    break;
  case SD_CARD_TYPE_SDHC:
    client.println("SDHC");
    break;
  default:
    client.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    client.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  long volumesize;
  client.print("\nVolume type is FAT");
  client.println(volume.fatType(), DEC);
  client.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  client.print("Volume size (bytes): ");
  client.println(volumesize);
  client.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  client.println(volumesize);
  client.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  client.println(volumesize);

  client.println("</pre>");

  listFiles(client, "", LS_SIZE);

}

// FLAGS
// LS_SIZE
// LS_DATE


void listFiles(Client client, String path, uint8_t flags) {
  // This code is just copied from SdFile.cpp in the SDFat library
  // and tweaked to print to the client output in html!
  
  // TODO reimplement the LS_R flag (recursive list of directories)

  SdFile root;
  if (path=="") {
    if (!root.openRoot(&volume)) error("openRoot failed");
  } else {
    // TODO we need to deal with the path
  }

  dir_t p;

  root.rewind();
  client.println("<ul>");
  while (root.readDir(p) > 0) {
    // done if past last used entry
    if (p.name[0] == DIR_NAME_FREE) break;

    // skip deleted entry and entries for . and  ..
    if (p.name[0] == DIR_NAME_DELETED || p.name[0] == '.') continue;

    // only list subdirectories and files
    if (!DIR_IS_FILE_OR_SUBDIR(&p)) continue;

    // print any indent spaces
    client.print("<li><a href=\"");
    for (uint8_t i = 0; i < 11; i++) {
      if (p.name[i] == ' ') continue;
      if (i == 8) {
        client.print('.');
      }
      client.print(p.name[i]);
    }
    client.print("\">");

    // print file name with possible blank fill
    for (uint8_t i = 0; i < 11; i++) {
      if (p.name[i] == ' ') continue;
      if (i == 8) {
        client.print('.');
      }
      client.print(p.name[i]);
    }

    client.print("</a>");

    if (DIR_IS_SUBDIR(&p)) {
      client.print('/');
    }

    // print modify date/time if requested
    if (flags & LS_DATE) {
      root.printFatDate(p.lastWriteDate);
      client.print(' ');
      root.printFatTime(p.lastWriteTime);
    }
    // print size if requested
    if (!DIR_IS_SUBDIR(&p) && (flags & LS_SIZE)) {
      client.print(' ');
      client.print(p.fileSize);
    }
    client.println("</li>");
    
        // list subdirectory content if requested    
  }
  client.println("</ul>");
}





