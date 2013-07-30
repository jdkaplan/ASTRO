#ifndef MOTORS_H
#define MOTORS_H

#define N_PULSES_1 90*2
#define N_PULSES_2 90*4
#define T_PULSE_MS 7

#define DIRONE P1DIR
#define PORTONE P1OUT
#define SHIFTONE 0

#define DIRTWO P1DIR
#define PORTTWO P1OUT
#define SHIFTTWO 4

void turnOne(char);
void turnTwo(char);

#define CLOSE2 2
#define OPEN 1
#define CLOSE 0

void setupMotors();

#endif
