#include <msp430.h>
#include "serial.h"
#include "timer.h"
#include "motors.h"
#include "hard.h"

int main() {
  // Stop watchdog timer
  WDTCTL = WDTPW + WDTHOLD;
  setupMotors();
  serialStart();
  timerStart();
  adcStart();

  _EINT();

  __bis_SR_register(LPM0_bits + GIE);
}
