#include <msp430.h>
#include "timer.h"
#include "serial.h"

long int timerMS;

void timerStart() {
  timerMS = 0;
  TACCR0 = TICKS_PER_MS;
  TAR = 0;
  TACTL = TASSEL_2 + MC_1 + TAIE + ID_3;
}

void (*executable)();

void executeAfterMS(int msTime, void (*fPtr)()) {
  executable = fPtr;
  TBCCR0 = msTime*TICKS_PER_MS;
  TBR = 0;
  TBCTL = TBSSEL_2 + MC_1 + TAIE + ID_3;
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

#pragma vector=TIMERB1_VECTOR
__interrupt void Timer_B(void) {
  switch(TAIV) { // Efficient switch-implementation
    case  2: break; // TACCR1 not used
    case  4: break; // TACCR2 not used
    case 10:
      TBCTL = 0;
      (*executable)();
      break;
    }
}
