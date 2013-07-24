#include <msp430f2274.h>

#define CYCLES_PER_MS 1000

void kill_watchdog() {
  WDTCTL = WDTPW + WDTHOLD;
}

void delay(unsigned int ms) {
  while(ms--) {
    __delay_cycles(CYCLES_PER_MS);
  }
}

void motor_cycle() {
  int i;
  for (i=0 ; i<100 ; i++) {
    int timing = 20;

    // 1 0 1 0
    P1OUT = 0b1010;
    delay(timing);

    // 1 0 0 1
    P1OUT = 0b1001;
    delay(timing);

    // 0 1 0 1
    P1OUT = 0b0101;
    delay(timing);

    // 0 1 1 0
    P1OUT = 0b0110;
    delay(timing);
  }
}

int main(void) {
  kill_watchdog();
  P1DIR = 0b1111;
  motor_cycle();
}
