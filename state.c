#include <msp430.h>
#include "state.h"

volatile stateVec globalState;

void saveState() {
  
}

void retrieveState() {
  
}

long generateChecksum() {
  return
    globalState.currentStepOne^
    globalState.desiredStepOne^
    globalState.prevSignalOne^

    globalState.currentStepTwo^
    globalState.desiredStepTwo^
    globalState.prevSignalTwo^

    globalState.internalTime^
    globalState.externalTime^

    globalState.temperature^

    globalState.safemode;
}

char checkChecksum(long checksum) {
  return (generateChecksum() == checksum);
}
