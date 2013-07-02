#include <msp430.h>
#include "motors.h"
#include "timer.h"

// Using PORT1 and the 4 lower bits of PORT4 for motor driver.

void turn();
void reset();

void setupMotors() {
  DIRONE |= 0xF<<SHIFTONE;
  DIRTWO |= 0xF<<SHIFTTWO;
  reset();
}

char current;
char flip;
volatile char *port;
char shift;
int n_pulses;

/*
  dir selects which direction we want to turn
  (clockwise or counterclockwise)
  0 - clockwise
  1 - counterclockwise
*/
void startOne(char dir) {
  reset();
  port = &PORTONE;
  flip = FLIP_START;
  current = dir?CLOCKWISE:COUNTER_CLOCKWISE;
  //flip = dir?FLIP_CLOCK:FLIP_COUNTER;
  //current = START_DIR;
  shift = SHIFTONE;
  n_pulses = 0;
  turn();
}

/*
  dir selects which direction we want to turn
  (clockwise or counterclockwise)
  0 - clockwise
  1 - counterclockwise
*/
void startTwo(char dir) {
  reset();
  port = &PORTTWO;
  flip = FLIP_START;
  current = dir?CLOCKWISE:COUNTER_CLOCKWISE;
  shift = SHIFTTWO;
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
  char portstate = (*port);
  ++n_pulses;
  if(n_pulses == N_PULSES) {
    reset();
    return;
  }
  
  portstate &= ~(0xF<<shift);
  portstate |= (current<<shift);
  (*port) = portstate;

  current ^= flip;
  flip ^= 0xF;

  executeAfterMS(T_PULSE_MS,&turn);
}

void reset() {
  // Everything LOW
  
  PORTONE &= ~(0xF<<SHIFTONE);
  PORTTWO &= ~(0xF<<SHIFTTWO);
}
