#include <msp430.h>
#include "serial.h"
#include "timer.h"
#include "motors.h"
#include "hard.h"
#include "mainActions.h"
#include "state.h"

#define START_ATOMIC() __bic_SR_register(GIE)
#define END_ATOMIC() __bis_SR_register(GIE)

int main() {
  int local_nQueued;
  // Stop watchdog timer
  void (*action)();
  WDTCTL = WDTPW + WDTHOLD;
  //setupMotors();
  serialStart();
  //timerStart();
  //adcStart();
  retrieveState();
  _EINT();

  P1DIR |= 0x3;
  P1OUT = 0;
  
  while(1) {
    START_ATOMIC();
    local_nQueued = nQueued;
    END_ATOMIC();
    while(local_nQueued) {
      START_ATOMIC();
      --nQueued;
      local_nQueued = nQueued;
      action = mainActionsQ[bottom];
      bottom = (bottom+1)%N_ACTIONS;
      END_ATOMIC();
      (*action)();
    }
    __bis_SR_register(LPM3_bits | GIE);
  }
}
