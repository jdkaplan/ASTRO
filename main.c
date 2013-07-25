#include <msp430.h>
#include "serial.h"
#include "timer.h"
#include "motors.h"
#include "hard.h"
#include "mainActions.h"
#include "state.h"
#include "parsing.h"

#define START_ATOMIC() __bic_SR_register(GIE); __no_operation(); __no_operation()
#define END_ATOMIC() __bis_SR_register(GIE)

int main() {
  // Stop watchdog timer
  WDTCTL = WDTPW + WDTHOLD;
  while(CALBC1_1MHZ==0xFF);
  volatile int local_nQueued;
  void (*action)();
  //startHard();
  startFlash();
  //setupMotors();
  serialStart();
  timerStart();
  //adcStart();
  _EINT();

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
