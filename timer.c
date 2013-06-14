#include <msp430.h>
#include "timer.h"
#include "serial.h"

long int timerMS;

void timerStart() {
  P1DIR = 0x3;
  P1OUT = 0x1;
  timerMS = 0;
  TACCR0 = TICKS_PER_MS;
  TACTL = TASSEL_2 + MC_1 + TAIE + ID_2;
}

#pragma vector=TIMERA1_VECTOR
__interrupt void Timer_A(void) {
  switch(TAIV) {       // Efficient switch-implementation
    case  2: break;                         // TACCR1 not used
    case  4: break;                         // TACCR2 not used
    case 10:
      TAR = 0;
      ++timerMS;
      break;
    }
}
