#include <msp430.h>
#include "mainActions.h"

int bottom,top;
int nQueued = 0;
void (*mainActionsQ[N_ACTIONS])();

void doAction(void (*action)()) {
  ++nQueued;
  mainActionsQ[top] = action;
  top = (top+1)%N_ACTIONS;
}

void changeStatus() {
  P1OUT ^= 0x1;
}
