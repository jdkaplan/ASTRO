#include <msp430.h>
#include "state.h"
#include "motors.h"
#include "hard.h"
#include "logic.h"

void turnMotorsOnThreshold();
int aboveheight(long);
void temperatureControl();

void logisticalize() {
  // Safe mode override
  if(globalState.safemode) {
    return;
  }
  turnMotorsOnThreshold();
  temperatureControl();
}

void turnMotorsOnThreshold() {
  if (aboveHeight(HEIGHT1)) {
    turnOne(OPEN);
    turnTwo(OPEN);

    turnHVDC1On();
    turnHVDC2On();
  }
  else if (aboveHeight(HEIGHT0)) {
    turnOne(OPEN);
    turnTwo(CLOSE);

    turnHVDC1On();
    turnHVDC2Off();
  }
  else {
    turnOne(CLOSE);
    turnTwo(CLOSE);

    turnHVDC1Off();
    turnHVDC2Off();
  } 
}

int aboveHeight(long threshold) {
  int above = 1;
  int i;
  for (i=0 ; above && (i<N_HEIGHTS) ; i++) {
    above = above && (globalState.prevHeights[i] >= threshold);
  }
  return above;
}

void temperatureControl() {
  // safemode override
  if (globalState.temperature < TEMP0) {
    turnHeater1On();
    turnHeater2On();
  }
  else {
    turnHeater1Off();
    turnHeater2Off();
  }
}
