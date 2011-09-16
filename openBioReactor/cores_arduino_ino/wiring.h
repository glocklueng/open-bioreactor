#ifndef wiring_h
#define wiring_h

#include <wiring_private.h>

SIGNAL(TIMER0_OVF_vect);

unsigned long millis();

unsigned long micros();

void delay(unsigned long ms);

void delayMicroseconds(unsigned int us);

void init();

#endif