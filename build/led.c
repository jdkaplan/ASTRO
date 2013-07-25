/*    
msp430-gcc -mmcu=msp430f2013 led.c -o led
sudo mspdebug uif

msp430-gcc -mmcu=msp430f2274 led.c -o led
sudo mspdebug rf2500
*/

#include <msp430f2274.h>

#define CYCLES_PER_MS 1000

void delay(unsigned int);
void LED_on(unsigned int);
void LED_off(unsigned int);
void blink(unsigned int,unsigned int);

int main (void) {
  // Turn off watchdog timer
  WDTCTL = WDTPW + WDTHOLD;
  
  /* P1.0 output */
  P1DIR = 1;
  int i;
  while(1) {
    blink(1000,0);
    for (i=0 ; i<200 ; i++) {
      blink(1,4);
    }
    for (i=0 ; i<100 ; i++) {
      blink(1,9);
    }
    for (i=0 ; i<66 ; i++) {
      blink(1,14);
    }
    for (i=0 ; i<50 ; i++) {
      blink(1,19);
    }
    blink(0,1000);
    for (i=0 ; i<50 ; i++) {
      blink(1,19);
    }
    for (i=0 ; i<66 ; i++) {
      blink(1,14);
    }
    for (i=0 ; i<100 ; i++) {
      blink(1,9);
    }
    for (i=0 ; i<200 ; i++) {
      blink(1,4);
    }
  }
  return 0;
}

void delay(unsigned int ms) {
  while(ms--) {
    __delay_cycles(CYCLES_PER_MS);
  }
}

void LED_on(unsigned int ms) {
  P1OUT = 0x1;
  delay(ms);
}

void LED_off(unsigned int ms) {
  P1OUT = 0;
  delay(ms);
}

void blink(unsigned int on, unsigned int off) {
  LED_on(on);
  LED_off(off);
}
