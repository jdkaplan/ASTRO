#include <msp430.h>
#include "state.h"

void turnMotorsOnThreshold();
int aboveheight(long);
void temperatureControl();

void logisticalize() {
  turnMotorsOnThreshold();
  temperatureControl();
}

void turnMotorsOnThreshold() {
  // safemode override
  if (globalState.safemode) {
    return;
  }

  if (aboveHeight(HEIGHT3)) {
    doCommand(0x05); // shut1
    doCommand(0x06); // open2
  }
  else if (aboveHeight(HEIGHT2)) {
    doCommand(0x05); // shut1
    doCommand(0x07); // shut2
  }
  else if (aboveHeight(HEIGHT1)) {
    doCommand(0x04); // open1
    doCommand(0x07); // shut2
  }  
  else if (aboveHeight(HEIGHT0)) {
    doCommand(0x05); // shut1
    doCommand(0x07); // shut2
  } 
  else {
    // negative height?
  }
}

int aboveHeight(long threshold) {
  int above = 1;
  int i;
  for (i=0 ; above && i<N_HEIGHTS ; i++) {
    above = above && (globalState.prevHeights[i] > threshold);
  }
  return above;
}

void temperatureControl() {
  // safemode override
  if (globalState.safemode) {
    return;
  }
  
  if (globalState.temperature < TEMP0) {
    doCommand(0x0E);  // heat1
    doCommand(0x10);  // heat2
  }
  else {
    doCommand(0x0F);  // cool1
    doCommand(0x11);  // cool2
  }
}
