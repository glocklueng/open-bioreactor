#include <EXROM.h>

#include <SD.h>



// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelectSDcard = 4; // SD card on pin 4
const int chipSelectEthernet = 10; // Ethernet chip on pin 10
const int addrEepromMax = 4096;
int addrEepromCurrent = 0;
boolean DEBUG = true;


float writeOutData = 12.123456;


void setup()
{
  Serial.begin(9600);
  Serial.print("\n\nInitializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(chipSelectSDcard, OUTPUT); //pin 4 is for the SD card 
  pinMode(chipSelectEthernet, OUTPUT); //and pin 10 for the Ethernet chip
  // see if the card is present and can be initialized:
  digitalWrite(chipSelectEthernet, HIGH);
  if (!SD.begin(chipSelectSDcard)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  if(DEBUG) Serial.println("card initialized.");


  //reset the EEPROM iterator to 0
  addrEepromCurrent = 0;

  //writeToSdCard();
  readFromSdCard();


}


void writeToEeprom()
{

  //----------WRITE TO EERPOM---------
  // the EEPROM on the Arduino Mega 2560 is 4kb (4096 bytes)
  // we're using the EXPROM library to read/write to the EEPROM
  EXROM.write(addrEepromCurrent, writeOutData);
  if(DEBUG)
  {
    Serial.println("The EEPROM iterator is : ");
    Serial.println(addrEepromCurrent);
    Serial.print("The size of _float_ written to the eeprom is: ");
    Serial.println(sizeof(writeOutData));  
  }

  addrEepromCurrent += sizeof(writeOutData);

  if(DEBUG) 
  {
    Serial.print("The NEW eeprom iterator is : "); 
    Serial.println(addrEepromCurrent); 
  }


}


void readFromEeprom()
{
  float temp;

  if(DEBUG)
  {
    Serial.println("The EEPROM output is: \t");
    for(int i=0; i<19; i++)
    {
      EXROM.read(i,&temp);
      Serial.print(temp,9);
      Serial.print("\t");     
    }
  }



}


void writeToSdCard()
{
  //----------WRITE TO SD-CARD---------
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.


  // note that String is different of string (read online reference arduino.cc)
  String writeSDData = "This is only a test.";
  File dataFile;

  // If the file is opened for writing, it will be created if it doesn't already exist 
  // (but the directory containing it must already exist)
  if(DEBUG)
  {
    Serial.print("The file to be written already exists (1) or is inexistent (0): ");
    int i = SD.exists("/test2.html");
    Serial.println(i);
  }


  dataFile = SD.open("/test2.txt", FILE_WRITE); // always include the forward slash if data is in root folder

  // if the file is available, write to it:
  if (dataFile) 
  {
    if(DEBUG) Serial.println("Writing to SD card...");
    dataFile.println(writeSDData); // or string
    dataFile.close();
    // print to the serial port too:
    if(DEBUG) 
    {    
      Serial.println("The data written on the SD card is:");      
      Serial.println(writeSDData); 
      Serial.println("Finished writing to SD card.");
    }
  }  
  // if the file isn't open, pop up an error:
  else 
  {
    Serial.println("ERROR: opening datafile for writing!");
  } 


}

void readFromSdCard()
{
  if(DEBUG) Serial.println("Reading from SD card...");

  String webSiteBuffer = "";
  char temp;

  int i = 2;
  if(DEBUG)
  {
    Serial.print("/TEST.HTM already exists (1) or is inexistent (0): ");
    i = SD.exists("/TEST.HTM");
    Serial.println(i);
    //    Serial.print("TEST.HTM written already exists (1) or is inexistent (0): ");
    //    i = SD.exists("TEST.HTM");
    //    Serial.println(i);
    //    Serial.print("/datAlog.txt written already exists (1) or is inexistent (0): ");
    //    i = SD.exists("/datAlog.txt");
    //    Serial.println(i);
    //    Serial.print("datalog.txt written already exists (1) or is inexistent (0): ");
    //    i = SD.exists("datalog.txt");
    //    Serial.println(i);
    //    Serial.print("/datalog2.txt written already exists (1) or is inexistent (0): ");
    //    i = SD.exists("/datalog2.txt");
    //    Serial.println(i);
    //    Serial.print("datalog2.txt written already exists (1) or is inexistent (0): ");
    //    i = SD.exists("datalog2.txt");    
    //    Serial.println(i);
    //    Serial.print("DATALOG2.TXT written already exists (1) or is inexistent (0): ");
    //    i = SD.exists("DATALOG2.TXT");
    //    Serial.println(i);

  }
  unsigned long time =millis();
  File readFile = SD.open("test.htm", FILE_READ);

  if (readFile) {
    Serial.println("\nThe datafile contains: \n");

    // read from the file until there's nothing else in it:
    while (readFile.available()) {
      temp = readFile.read(); //read only reads one character
      //if(DEBUG) Serial.println(temp);
      webSiteBuffer += temp;
    }
    // close the file:
    readFile.close();

    //Serial.println(webSiteBuffer);

    unsigned long time2 = millis() - time;
    Serial.print("The reading prozess of the file took: [milli sec]");
    Serial.println(time2);


  } 
  else {
    // if the file didn't open, print an er  ror:
    Serial.println("ERROR: opening datafile for reading!");
  }


  //testing:

  Serial.print("Size of webSiteBuffer: ");
  Serial.println(webSiteBuffer.length());



  time =millis();

  File writeFile = SD.open("/test2.htm", FILE_WRITE);
  if(writeFile)
  {
    writeFile.print(webSiteBuffer);

  }
  else Serial.println("ERROR: writing file.");

  writeFile.close();
  Serial.println("Writing finished");
  unsigned long time3 = millis() - time;
  Serial.print("The writing prozess of the file took: [milli sec]");
  Serial.println(time3);





}




void loop()
{

//  if(DEBUG) Serial.println("Reading from SD card...");
//
//  String webSiteBuffer = "";
//  String verifyString = "";
//  char temp;
//  unsigned long time, timeDiff;
//
//  time =millis();
//  File readFile = SD.open("test.htm", FILE_READ);
//
//  if (readFile) {
//    Serial.println("\nThe datafile contains: \n");
//
//    // read from the file until there's nothing else in it:
//    while (readFile.available()) {
//      temp = readFile.read(); //read only reads one character
//      //Serial.println(temp);
//      webSiteBuffer += temp;
//    }
//    // close the file:
//    readFile.close();
//
//    //Serial.println(webSiteBuffer);
//
//    unsigned long timeDiff = millis() - time;
//    Serial.print("The writing prozess of the file took: [milli sec]");
//    Serial.println(timeDiff);
//
//
//  } 
//  else {
//    // if the file didn't open, print an error:
//    Serial.println("ERROR: opening datafile for reading!");
//  }
//
//
//  time =millis();
//
//  File writeFile = SD.open("/test2.htm", FILE_WRITE);
//  if(writeFile)
//  {
//    writeFile.print(webSiteBuffer);
//
//  }
//  else Serial.println("ERROR: writing file.");
//
//  writeFile.close();
//  Serial.println("Writing finished");
//  timeDiff = millis() - time;
//  Serial.print("The writing prozess of the file took: [milli sec]");
//  Serial.println(timeDiff);
//  
//  
//    //----------WRITE TO EERPOM---------
//  // the EEPROM on the Arduino Mega 2560 is 4kb (4096 bytes)
//  // we're using the EXPROM library to read/write to the EEPROM
//  Serial.println("Writing to EEPROM...");
//  char buff[100];
//  
//    if(DEBUG)
//  {
//    Serial.println("The EEPROM iterator is : ");
//    Serial.println(addrEepromCurrent);
//    Serial.print("The size of the data written to the eeprom is: ");
//    Serial.println(sizeof(webSiteBuffer));  
//  }
//  
//  time =millis();
//  for(int j = 0; j<2; j++)
//  {
//  webSiteBuffer.toCharArray(buff, sizeof(buff));
//  EXROM.write(addrEepromCurrent, buff, sizeof(buff));
//  addrEepromCurrent += sizeof(buff);
//  }
//  timeDiff = time - millis();
//  Serial.print("The EEPROM writing took [ms]: ");
//  Serial.println(timeDiff);
//
//
//
//
//  if(DEBUG) 
//  {
//    Serial.print("The NEW eeprom iterator is : "); 
//    Serial.println(addrEepromCurrent); 
//  }
//  
//  //READ from eeprom
//  
//    if(DEBUG)
//  {
//    Serial.println("The EEPROM output is: \t");
//    for(int i=0; i<1000; i++)
//    {
//      EXROM.read(i,&temp);
//      verifyString += temp;
//      Serial.print(temp);
//      Serial.print("\t");     
//    }
//  }
//  Serial.println("The EEPROM has been read and the output is: ");
//  Serial.print(verifyString);
//  




  return;
}










