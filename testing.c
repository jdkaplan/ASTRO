#include <msp430f2274.h>

#define CYCLES_PER_MS 1000

void delay(unsigned int);
void pin_on(int);
void pin_off(int);

void delay(unsigned int ms) {
    while(ms--) {
        __delay_cycles(CYCLES_PER_MS);
    }
}

void pin_on(int pin) {
  P1DIR |= pin;
}

void pin_off(int pin) {
  P1DIR &= ~pin;
}

void cycle_pin(int pin, unsigned int on, unsigned int off) {
  pin_on(pin);
  delay(on);
  pin_off(pin);
  delay(off);
}

int main(void) {
  // turn off watchdog timer
  WDTCTL = WDTPW + WDTHOLD;
  
  // everything off
  P1DIR = 0x00;
  P1SEL = 0x00;
  P2OUT = P1DIR;
  P2DIR = 0x00;
  P2SEL = 0x00;
  P2OUT = P2DIR;
  P3DIR = 0x00;
  P3SEL = 0x00;
  P3OUT = P3DIR;
  P4DIR = 0x00;
  P4SEL = 0x00;
  P4OUT = P4DIR;

  while (1) {}
}
