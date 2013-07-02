#include <msp430f2274.h>

#define CYCLES_PER_MS 1000

void delay(unsigned int ms) {
  while(ms--) {
    __delay_cycles(CYCLES_PER_MS);
  }
}

// pin_cycle
// turns P1OUT on, then off
// on, off = time in ms
void toggle_pins_out (unsigned int on, unsigned int off) {
  P1OUT = P1DIR; // Power output pins
  delay(on);
  P1OUT = 0;
  delay(off);
}

int main(void) {
  // turn off watchdog timer
  WDTCTL = WDTPW + WDTHOLD;
    
  // P1.3 output
  P1DIR |= 0x03; // 
  P1SEL |= 0x00; // select all pins for I/O
    
  // turn off everything initially
  P1OUT = 0;
    
  // cycle pin for 1s/1s
  // int i;
  while(1) {
    P1DIR |= 0x03;
    P1DIR |= BIT7;
    P1DIR |= BIT6;
    P1OUT = P1DIR;
    P1DIR &= ~BIT7;
    P1DIR &= ~BIT6;
    
    delay(1000);
    P1DIR = 
  }
}

// 0x09 = 1.0 and 1.3 as output
// 0x81 = 1.0 and 1.7 as output
