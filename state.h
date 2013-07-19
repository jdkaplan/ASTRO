#ifndef STATE_H
#define STATE_H

#define DEBUG_MODE 1

typedef struct {
  short currentStepOne;
  short desiredStepOne;
  char prevSignalOne;

  short currentStepTwo;
  short desiredStepTwo;
  char prevSignalTwo;

  long internalTime;
  long externalTime;

  short temperature;

  char safemode;

} stateVec;

extern stateVec globalState;

void saveState();
void retrieveState();

#endif
