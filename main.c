#include <msp430.h>
#include "serial.h"
#include "timer.h"

int main() {
  // Stop watchdog timer
  WDTCTL = WDTPW + WDTHOLD;
  P1DIR = 0x3;
  P1OUT = 0x1;
  _EINT();
  serialStart();
  timerStart();

  while(1);
}
