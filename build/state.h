#ifndef STATE_H
#define STATE_H

#define DEBUG_MODE 1
#define N_HEIGHTS 5

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
  char heaterOne;
  char heaterTwo;

  char HVDCOne;
  char HVDCTwo;

  char safemode;
  
  long height;
  long prevHeights[N_HEIGHTS];

} stateVec;

extern stateVec globalState;

void startFlash();
void saveState();

#endif
