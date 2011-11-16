// The idea is to be able to log all the information

// In this class we will all the important information

// We could create a different file and directory structure per year / month / day
// This would still require an accurate time !!!

int stateIO[NUMBER_IO];
int stateAD[NUMBER_AD];

// Based on interrupt to have exact timing we will retrieve the inforamtion
void updateState() {
    for (int i=0; i<NUMBER_IO; i++) {
    stateIO[i]=digitalRead(i);
  }
     for (int i=0; i<NUMBER_AD; i++) {
    stateAD[i]=analogRead(i);
  }
  // we need to store the timestamp that will be saved in the datalog file
}


// This function is called when we have the time ...
void storeState() {
  // we store in the log file all the information
  
  // could also store the number of SimpleTimer
}

