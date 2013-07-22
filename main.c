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
  setupMotors();
  serialStart();
  timerStart();
  adcStart();

  _EINT();

  P1DIR |= 0x1;
  
  while(1) {
    while(nQueued) {
      _BIC_SR(GIE);
      --nQueued;
      action = mainActionsQ[bottom];
      bottom = (bottom+1)%N_ACTIONS;
      _BIS_SR(GIE);
      (*action)();
    }
    __bis_SR_register(LPM0_bits | GIE);
  }
}
