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

void set_pins() {
  P1OUT = P1DIR;
  P2OUT = P2DIR;
  P3OUT = P3DIR;
  P4OUT = P4DIR;
}

/* void cycle_pin(int pin, unsigned int on, unsigned int off) { */
/*   pin_on(pin); */
/*   set_pins(); */
/*   delay(on); */
/*   pin_off(pin); */
/*   set_pins(); */
/*   delay(off); */
/* } */

/* void pin_on_2(int pin) { */
/*   P1DIR |= pin; */
/* } */

/* void pin_off_2(int pin) { */
/*   P2DIR &= ~pin; */
/* } */

/* void cycle_pin_2(int pin, unsigned int on, unsigned int off) { */
/*   pin_on_2(pin); */
/*   set_pins(); */
/*   delay(on); */
/*   pin_off_2(pin); */
/*   set_pins(); */
/*   delay(off); */
/* } */

// pin_01 = GND
// pin_02 = VCC

void pin_03(bool state) {
  P2DIR |= ((int)state * 0x01);
  P2DIR &= ~((1 - (int)state) * 0x01);
}

void pin_04(bool state) {
  P2DIR |= ((int)state * 0x02);
  P2DIR &= ~((1 - (int)state) * 0x02);
}

void pin_05(bool state) {
  P2DIR |= ((int)state * 0x04);
  P2DIR &= ~((1 - (int)state) * 0x04);
}

void pin_06(bool state) {
  P2DIR |= ((int)state * 0x08);
  P2DIR &= ~((1 - (int)state) * 0x08);
}

void pin_07(bool state) {
  P2DIR |= ((int)state * 0x10);
  P2DIR &= ~((1 - (int)state) * 0x10);
}

void pin_08(bool state) {
  P4DIR |= ((int)state * 0x08);
  P4DIR &= ~((1 - (int)state) * 0x08);
}

void pin_09(bool state) {
  P4DIR |= ((int)state * 0x10);
  P4DIR &= ~((1 - (int)state) * 0x10);
}

void pin_10(bool state) {
  P4DIR |= ((int)state * 0x20);
  P4DIR &= ~((1 - (int)state) * 0x20);
}

void pin_11(bool state) {
  P4DIR |= ((int)state * 0x40);
  P4DIR &= ~((1 - (int)state) * 0x40);
}

// pin_12 = GND

void pin_13(bool state) {
  P2DIR |= ((int)state * 0x40);
  P2DIR &= ~((1 - (int)state) * 0x40);
}

void pin_14(bool state) {
  P2DIR |= ((int)state * 0x80);
  P2DIR &= ~((1 - (int)state) * 0x80);
}

void pin_15(bool state) {
  P3DIR |= ((int)state * 0x04);
  P3DIR &= ~((1 - (int)state) * 0x04);
}

void pin_16(bool state) {
  P3DIR |= ((int)state * 0x08);
  P3DIR &= ~((1 - (int)state) * 0x08);
}

void pin_17(bool state) {
  P3DIR |= ((int)state * 0x01);
  P3DIR &= ~((1 - (int)state) * 0x01);
}

void pin_18(bool state) {
  P3DIR |= ((int)state * 0x02);
  P3DIR &= ~((1 - (int)state) * 0x02);
}


int main(void) {
  // turn off watchdog timer
  WDTCTL = WDTPW + WDTHOLD;
  
  // everything off
  /*
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
  */

  P2SEL = 0;
  while (1) {
    //cycle_pin_2(0x1, 1000, 1000);
    P1DIR = 0x1;
    P1OUT = P1DIR;
    P2DIR = 0x1;
    P2OUT = P2DIR;
    delay(1000);
    
    P1DIR = 0x0;
    P1OUT = P1DIR;
    P2DIR = 0x0;
    P2OUT = P2DIR;
    delay(1000);
    
  }
}
