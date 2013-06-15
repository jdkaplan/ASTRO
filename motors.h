#ifndef MOTORS_H
#define MOTORS_H

#define N_PULSES 90*4
#define T_PULSE_MS 20

#define CLOCKWISE 0b1010
#define COUNTER_CLOCKWISE 0b0110
#define FLIP_START 0b11

#define PORTONE P1OUT
#define PORTTWO P1OUT
#define PORTTHREE P4OUT

#define SHIFTONE 0
#define SHIFTTWO 4
#define SHIFTTHREE 0

void setupMotors();

void startOne();
void startTwo();
void startThree();

#endif
