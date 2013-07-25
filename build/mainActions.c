#include <msp430.h>
#include "mainActions.h"

volatile int bottom,top;
volatile int nQueued = 0;
void (*volatile mainActionsQ[N_ACTIONS])();

// Always called inside interrupt - schedules a function to be run on main.
void doAction(void (*action)()) {
  ++nQueued;
  if(nQueued == N_ACTIONS)  {
    P1OUT |= 0x1;
  }
  mainActionsQ[top] = action;
  top = (top+1)%N_ACTIONS;
}
