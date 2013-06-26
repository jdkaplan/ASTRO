#include <msp430.h>
#include "hard.h"

void startHard() {
  turnHDVC1Off();
  turnHDVC2Off();
  turnHeaterOff();
  HDVC1DIR |= HDVC1PIN;
  HDVC2DIR |= HDVC2PIN;
  HEATERDIR |= HEATERPIN;
}

void turnHDVC1On() {
  HDVC1OUT |= HDVC1PIN;
}

void turnHDVC1Off() {
  HDVC1OUT &= ~(HDVC1PIN);
}

void turnHDVC2On() {
  HDVC2OUT |= HDVC2PIN;
}

void turnHDVC2Off() {
  HDVC2OUT &= ~HDVC2PIN;
}

void turnHeaterOn() {
  HEATEROUT |= HEATERPIN;
}

void turnHeaterOff() {
  HEATEROUT &= ~HEATERPIN;
}
