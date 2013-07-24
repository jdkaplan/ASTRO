#ifndef MOTORS_H
#define MOTORS_H

#define N_PULSES 90*4
#define T_PULSE_MS 100

#define CLOCKWISE 0b0101
#define COUNTER_CLOCKWISE 0b0110
#define FLIP_START 0b11

#define DIRONE P1DIR
#define DIRTWO P1DIR

#define PORTONE P1OUT
#define PORTTWO P1OUT

#define SHIFTONE 0
#define SHIFTTWO 4

void setupMotors();

void startOne(char);
void startTwo(char);

#endif
