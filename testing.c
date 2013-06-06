#include <msp430f2274.h>

#define CYCLES_PER_MS 1000


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

// pin_01 = GND
// pin_02 = VCC

void pin_03(int state) {
  P2DIR |= (state * 0x01);
  P2DIR &= ~((1 - state) * 0x01);
}

void pin_04(int state) {
  P2DIR |= (state * 0x02);
  P2DIR &= ~((1 - state) * 0x02);
}

void pin_05(int state) {
  P2DIR |= (state * 0x04);
  P2DIR &= ~((1 - state) * 0x04);
}

void pin_06(int state) {
  P2DIR |= (state * 0x08);
  P2DIR &= ~((1 - state) * 0x08);
}

void pin_07(int state) {
  P2DIR |= (state * 0x10);
  P2DIR &= ~((1 - state) * 0x10);
}

void pin_08(int state) {
  P4DIR |= (state * 0x08);
  P4DIR &= ~((1 - state) * 0x08);
}

void pin_09(int state) {
  P4DIR |= (state * 0x10);
  P4DIR &= ~((1 - state) * 0x10);
}

void pin_10(int state) {
  P4DIR |= (state * 0x20);
  P4DIR &= ~((1 - state) * 0x20);
}

void pin_11(int state) {
  P4DIR |= (state * 0x40);
  P4DIR &= ~((1 - state) * 0x40);
}

// pin_12 = GND

void pin_13(int state) {
  P2DIR |= (state * 0x40);
  P2DIR &= ~((1 - state) * 0x40);
}

void pin_14(int state) {
  P2DIR |= (state * 0x80);
  P2DIR &= ~((1 - state) * 0x80);
}

void pin_15(int state) {
  P3DIR |= (state * 0x04);
  P3DIR &= ~((1 - state) * 0x04);
}

void pin_16(int state) {
  P3DIR |= (state * 0x08);
  P3DIR &= ~((1 - state) * 0x08);
}

void pin_17(int state) {
  P3DIR |= (state * 0x01);
  P3DIR &= ~((1 - state) * 0x01);
}

void pin_18(int state) {
  P3DIR |= (state * 0x02);
  P3DIR &= ~((1 - state) * 0x02);
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
  
  while (1) {
    
    // 3,5,7,9
    int timing = 100;

    // 1 0 1 0
    pin_03(1);
    pin_07(1);
    set_pins();
    delay(timing);

    // 1 0 0 1
    pin_07(0);
    pin_09(1);
    set_pins();
    delay(timing);

    // 0 0 1 1
    pin_07(0);
    pin_05(1);
    set_pins();
    delay(timing);

    // 0 1 1 0
    pin_09(0);
    pin_07(1);
    set_pins();
    delay(timing);
  }
}
