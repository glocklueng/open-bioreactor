void getStatus(Print* output) {
    // temporary buffer for sprintf
  char buf[10];
  
  output->print("FreeRam: ");
  output->println(FreeRam());
  output->print("Milli: ");
  output->println(millis());
  output->print("Micro: ");
  output->println(micros());

  output->println("\r\nDIGITAL INPUT/OUTPUT:");
  for (int i=0; i<NUMBER_IO; i++) {
    if (i%8==0) {
      if (i>0) {
        output->println(""); 
      } 
      if (i<10) {
        output->print(" "); 
      }
      output->print(i);
      output->print(": ");
    }   
    output->print(digitalRead(i));
    output->print(" ");
  }
  output->println("\r\n\r\nANALOGIC INPUT:");
  for (int i=0; i<NUMBER_AD; i++) {
    if (i%8==0) {
      if (i>0) {
        output->println(""); 
      }
      if (i<10) {
        output->print(" "); 
      }
      output->print(i);
      output->print(": ");
    }
    sprintf(buf, "%5.0i", analogRead(i));
    output->print(buf);
  }
  output->println("\r\n\r\nTEMPERATURES:");
  getAllSensorsStatus(output);

  output->println("\r\nEEPROM:");
  getStatusEEPROM(output);

  output->println("\r\nEVENTS:");
  getEventLogs(output);

  output->println("\r\nQUEUE:");
  timer.getStatus(output);
}






