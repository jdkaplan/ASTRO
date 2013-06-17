#include <msp430.h>
#include "serial.h"
#include "timer.h"

int main() {
  // Stop watchdog timer
  WDTCTL = WDTPW + WDTHOLD;
  _EINT();
  setupMotors();
  P1OUT = 0x1;
  serialStart();
  timerStart();

  while(1);
}
