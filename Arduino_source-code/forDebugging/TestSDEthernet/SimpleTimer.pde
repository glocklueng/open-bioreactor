SimpleTimer timer;

// We could improve the library to:
// * list the queue
// * add a int parameter to the function ???

// We execute the timer when we have the time ... There is a call in the main loop to check if there
// is anything to do.
// This is in contrast with "Interrupt"


// By default this library has a MAX_TIMERS=10
// This could be increase to 50 !

void setupSimpleTimer() {
    // timed actions setup
//  timer.setInterval(15000, RepeatTask);
//  timer.setTimeout(10000, OnceOnlyTask);
//  timer.setTimer(1200, TenTimesTask, 10);
}

void loopSimpleTimer() {
   timer.run();
}


// function to be called repeatedly
void RepeatTask() {
  Serial.println("15 second timer");        
}

// function to be called just once
void OnceOnlyTask() {
  Serial.println("This timer only triggers once");  
}

// function to be called exactly 10 times
void TenTimesTask() {
  static int k = 0;
  k++;
  Serial.print("called ");
  Serial.print(k);
  Serial.println(" / 10 times.");
}
