#include <msp430.h>
#include "timer.h"
#include "serial.h"
#include "state.h"
#include "mainActions.h"
#include "logic.h"

volatile long int timerMS;

void timerStart() {
  timerMS = 0;
  TACCR0 = TICKS_PER_MS;
  TAR = 0;
  TACTL = TASSEL_1 + MC_1 + TAIE;
}

void (*executable)();

long overflow;

void executeAfterMS(int msTime, void (*fPtr)()) {
  // Note: convert from 1/1024th of a second to MS
  long val = (((long)msTime)*((long)TICKS_PER_MS)*1024l)/(1000l);
  overflow = (val > 0xFFFF)?(val-0xFFFF):0;
  executable = fPtr;
  TBCCR0 = (val>0xFFFF)?0xFFFF:val;
  TBR = 0;
  TBCTL = TBSSEL_1 + MC_1 + TAIE;
}

#pragma vector=TIMERA1_VECTOR
__interrupt void Timer_A(void) {
  switch(TAIV) {       // Efficient switch-implementation
    case  2: break;                         // TACCR1 not used
    case  4: break;                         // TACCR2 not used
    case 10:
      TAR = 0;
      ++globalState.internalTime;
      /* Execute logic every 0x100 MS (256) */
      if(!(globalState.internalTime&0x100)) {
	logisticalize();
      }
      break;
  }
}

#pragma vector=TIMERB1_VECTOR
__interrupt void Timer_B(void) {
  switch(TAIV) { // Efficient switch-implementation
    case  2: break; // TBCCR1 not used
    case  4: break; // TBCCR2 not used
    case 10:
      TBCTL = 0;      
      if(!overflow) {
	(*executable)();
      }
      else {
	executeAfterMS(overflow/TICKS_PER_MS,executable);
      }
      break;
    }
}
