#include <msp430.h>
#include "motors.h"
#include "timer.h"

// Using PORT1 and the 4 lower bits of PORT4 for motor driver.

void turn();
void reset();

void setupMotors() {
  P1DIR = 0xFF;
  reset();
}

char current;
char flip;
volatile char *port;
char shift;
int n_pulses;

char prevDirOne = COUNTER_CLOCKWISE;
void startOne() {
  reset();
  port = &PORTONE;
  flip = FLIP_START;
  shift = SHIFTONE;
  prevDirOne ^= 0b1100;
  current = prevDirOne;
  n_pulses = 0;
  turn();
}

char prevDirTwo = COUNTER_CLOCKWISE;
void startTwo() {
  reset();
  port = &PORTTWO;
  flip = FLIP_START;
  shift = SHIFTTWO;
  prevDirTwo ^= 0b1100;
  current = prevDirTwo;
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
  (*port) &= (0xF0>>shift);
  (*port) |= (current<<shift);
  
  current ^= flip;
  flip ^= 0xF;

  ++n_pulses;
  if(n_pulses != N_PULSES) {
    executeAfterMS(T_PULSE_MS,&turn);
  }
  else {
    serialSend("Finished turning motor\n");
    reset();
  }
}

void reset() {
  // Everything LOW
  P1OUT = 0x00;
  P4OUT &= 0xF0;
}
