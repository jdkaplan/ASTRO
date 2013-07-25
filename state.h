#ifndef STATE_H
#define STATE_H

#define DEBUG_MODE 1

typedef struct {
  short currentStepOne;
  short desiredStepOne;
  char curSignalOne;

  short currentStepTwo;
  short desiredStepTwo;
  char curSignalTwo;

  long internalTime;
  long externalTime;

  short temperature;

  char safemode;
  
  long height;

} stateVec;

extern stateVec globalState;

void startFlash();
void saveState();

#endif
