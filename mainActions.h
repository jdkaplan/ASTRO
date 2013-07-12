#ifndef MAIN_ACTIONS_H
#define MAIN_ACTIONS_H

#define N_ACTIONS 30

extern int bottom,top;
extern int nQueued;
extern void (*mainActionsQ[])();

void doAction(void (*)());
void changeStatus();

#endif
