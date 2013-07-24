#include <msp430.h>
#include "serial.h"
#include "timer.h"
#include "motors.h"
#include "hard.h"
#include "mainActions.h"
#include "state.h"

int main() {
  // Stop watchdog timer
  void (*action)();
  WDTCTL = WDTPW + WDTHOLD;
  //setupMotors();
  serialStart();
  timerStart();
  //adcStart();
  retrieveState();
  _EINT();

  P1DIR |= 0x3;
  P1OUT = 0;
  
  while(1) {
    while(nQueued) {
      --nQueued;
      action = mainActionsQ[bottom];
      bottom = (bottom+1)%N_ACTIONS;
      (*action)();
    }
    __bis_SR_register(LPM0_bits | GIE);
  }
}
