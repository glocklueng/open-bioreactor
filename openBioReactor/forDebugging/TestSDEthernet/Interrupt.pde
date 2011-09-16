// We create an interrupt using the Timer2. This disable PWM on pin 9 and 10

// we can use this to make a watchDog ... In the main loop a counter could be incremented and check here.

void setupInterrupt() {
  MsTimer2::set(5000, interrupt); // 5000ms period
  MsTimer2::start();
}

void interrupt() {
  // we could load here all the information in "State"
  // we would save to SD card the state later ...
  
  Serial.print("Interrupt: ");
  Serial.println(millis()); 
}

