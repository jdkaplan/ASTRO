#include <msp430.h>
#include "serial.h"
#include "timer.h"

int main() {
  // Stop watchdog timer
  WDTCTL = WDTPW + WDTHOLD;
  P1DIR = 0x1;
  _EINT();
  serialStart();
  timerStart();
  long prev = 0;
  while(1) {
    if(timerMS/1000 > prev) {
      serialSend("Hess");
      P1OUT ^= 0x3;
      prev = timerMS/1000;
    }
  }
}
