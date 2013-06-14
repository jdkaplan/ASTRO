#include <msp430.h>
#include "serial.h"
#include "timer.h"

int main() {
  // Stop watchdog timer
  WDTCTL = WDTPW + WDTHOLD;
  _EINT();
  serialStart();
  timerStart();
  long prev = 0;
  while(1) {
    if(timerMS/1000 > prev) {
      serialSend("Hello, how are you?");
      P1OUT ^= 0x3;
      prev = timerMS/1000;
    }
  }
}
