#ifndef MAIN_ACTIONS_H
#define MAIN_ACTIONS_H

#define N_ACTIONS 30

extern volatile int bottom,top;
extern volatile int nQueued;
extern void (* volatile mainActionsQ[])();

void doAction(void (*)());

#endif
