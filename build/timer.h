#ifndef TIMER_H
#define TIMER_H

#define T_CLK (32768l)
#define DIV (1l)
// New MS: 1/1024th of a second
#define TICKS_PER_MS ((int)(T_CLK/(DIV*1024)))

extern volatile long int timerMS;

void timerStart();
void executeAfterMS(int,void (*)());

#endif
