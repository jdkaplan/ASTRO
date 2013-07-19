#ifndef STATE_H
#define STATE_H

typedef struct {
  short currentStepOne;
  short desiredStepOne;
  char prevSignalOne;

  short currentStepTwo;
  short desiredStepTwo;
  byte prevSignalTwo;

  long internalTime;
  long externalTime;

  short temperature;

  char safemode;

} stateVec;

extern volatile stateVec globalState;

void saveState();
void retrieveState();

#endif
