#include <msp430f2274.h>

#define CYCLES_PER_MS 1000

void kill_watchdog() {
  WDTCTL = WDTPW + WDTHOLD;
}

void reset() {
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
}

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

void red_led(int state) {
  P1DIR |= (state * 0x1);
  P1DIR &= ~((1 - state) * 0x1);
}

void green_led(int state) {
  P1DIR |= (state * 0x2);
  P1DIR &= ~((1 - state) * 0x2);
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

void motor_cycle() {
  int i;
  for (i=3 ; i>0 ; i--) {
    // 3,5,7,9
    int timing = 100;

    // 1 0 1 0
    pin_03(1);
    pin_05(0);
    pin_07(1);
    pin_09(0);

    red_led(1);
    green_led(0);

    set_pins();
    delay(timing);

    // 1 0 0 1
    pin_03(1);
    pin_05(0);
    pin_07(0);
    pin_09(1);

    red_led(0);
    green_led(1);

    set_pins();
    delay(timing);

    // 0 1 0 1
    pin_03(0);
    pin_05(1);
    pin_07(0);
    pin_09(1);

    red_led(1);
    green_led(1);

    set_pins();
    delay(timing);

    // 0 1 1 0
    pin_03(0);
    pin_05(1);
    pin_07(1);
    pin_09(0);

    red_led(0);
    green_led(0);

    set_pins();
    delay(timing);
  }
}

int main(void) {
  kill_watchdog();
  reset();

  motor_cycle();
}
