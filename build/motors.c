#include <msp430.h>
#include "motors.h"
#include "timer.h"
#include "state.h"

void motorMonitor();

void setupMotors() {
  globalState.curSignalOne = 0;
  globalState.curSignalTwo = 0;
  DIRONE |= 0xF<<SHIFTONE;
  DIRTWO |= 0xF<<SHIFTTWO;
  motorMonitor();
}

void turnOne(char dir) {
  globalState.desiredStepOne = dir*N_PULSES;
}

void turnTwo(char dir) {
  globalState.desiredStepTwo = dir*N_PULSES;
}

/*
  Sequence:
  0b1010
  0b1001
  0b0101
  0b0110

  (reverse for other direction)
*/

char increaseSignal[] = {
  0b1010, // 0b0000
  0b0000, // 0b0001
  0b0000, // 0b0010
  0b0000, // 0b0011
  0b0000, // 0b0100
  0b0110, // 0b0101
  0b1010, // 0b0110
  0b0000, // 0b0111
  0b0000, // 0b1000
  0b0101, // 0b1001
  0b1001  // 0b1010
};

char decreaseSignal[] = {
  0b1010, // 0b0000
  0b0000, // 0b0001
  0b0000, // 0b0010
  0b0000, // 0b0011
  0b0000, // 0b0100
  0b1001, // 0b0101
  0b0101, // 0b0110
  0b0000, // 0b0111
  0b0000, // 0b1000
  0b1010, // 0b1001
  0b0110  // 0b1010
};

void motorMonitor() {
  char portstate;
  // Motor one
  // Increase signal
  if(globalState.currentStepOne < globalState.desiredStepOne) {
    globalState.curSignalOne = increaseSignal[globalState.curSignalOne];
    ++globalState.currentStepOne;
  }
  // Decrease Signal
  else if(globalState.currentStepOne > globalState.desiredStepOne) {
    globalState.curSignalOne = decreaseSignal[globalState.curSignalOne];
    --globalState.currentStepOne;
  }
  // Stay the same
  else {
    globalState.curSignalOne = 0;
  }
  portstate = PORTONE;
  portstate &= ~(0xF<<SHIFTONE);
  portstate |= (globalState.curSignalOne<<SHIFTONE);
  PORTONE = portstate;

  // Motor Two
  // Increase signal
  if(globalState.currentStepTwo < globalState.desiredStepTwo) {
    globalState.curSignalTwo = increaseSignal[globalState.curSignalTwo];
    ++globalState.currentStepTwo;
  }
  // Decrease Signal
  else if(globalState.currentStepTwo > globalState.desiredStepTwo) {
    globalState.curSignalTwo = decreaseSignal[globalState.curSignalTwo];
    --globalState.currentStepTwo;
  }
  // Stay the same
  else {
    globalState.curSignalTwo = 0;
  }
  portstate = PORTTWO;
  portstate &= ~(0xF<<SHIFTTWO);
  portstate |= (globalState.curSignalTwo<<SHIFTTWO);
  PORTTWO = portstate;

  executeAfterMS(T_PULSE_MS, &motorMonitor);
}
