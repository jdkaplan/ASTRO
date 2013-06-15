#include <msp430.h>
#include "motors.h"
#include "timer.h"

// Using PORT1 and the 4 lower bits of PORT4 for motor driver.

void turn();
void reset();

void setupMotors() {
  P1DIR = 0xFF;
  P4DIR |= 0x0F;
  reset();
}

char current;
char flip;
volatile char *port;
char shift;
int n_pulses;

char prevDirOne = COUNTER_CLOCKWISE;
void startOne() {
  port = &PORTONE;
  flip = FLIP_START;
  shift = SHIFTONE;
  current = prevDirOne^(0b1100);
  n_pulses = 0;
  turn();
}

char prevDirTwo = COUNTER_CLOCKWISE;
void startTwo() {
  port = &PORTTWO;
  flip = FLIP_START;
  shift = SHIFTTWO;
  current = prevDirTwo^(0b1100);  
  n_pulses = 0;
  turn();
}

char prevDirThree = COUNTER_CLOCKWISE;
void startThree() {
  port = &PORTTHREE;
  flip = FLIP_START;
  shift = SHIFTTHREE;
  current = prevDirThree^(0b1100);
  n_pulses = 0;
  turn();
}

/*
  Sequence:
  0b1010
  0b1001
  0b0101
  0b0110

  (reverse for other direction)
*/
void turn() {
  (*port) &= (current<<shift) & 0xFF;
  (*port) |= (current<<shift);
  
  current ^= flip;
  flip ^= 0xF;
  ++n_pulses;
  if(n_pulses != N_PULSES) {
    executeAfterMS(T_PULSE_MS,&turn);
  }
  else {
    reset();
  }
}

void reset() {
  // Everything LOW
  P1OUT = 0x00;
  P4OUT &= 0xF0;
}
