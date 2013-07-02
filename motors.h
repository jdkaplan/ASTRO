#ifndef MOTORS_H
#define MOTORS_H

#define N_PULSES 90*4
#define T_PULSE_MS 100

#define CLOCKWISE 0b0101
#define COUNTER_CLOCKWISE 0b0110
#define FLIP_START 0b11

#define DIRONE P2DIR
#define DIRTWO P4DIR

#define PORTONE P2OUT
#define PORTTWO P4OUT

#define SHIFTONE 1
#define SHIFTTWO 3

void setupMotors();

void startOne(char);
void startTwo(char);

#endif
