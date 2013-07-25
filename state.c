#include <msp430.h>
#include "state.h"

stateVec globalState;
char *data = (char *)(&globalState);

char checkChecksum(long, stateVec *);
long generateChecksum(stateVec *);
void newStateVec();

#define START_ATOMIC() __bic_SR_register(GIE)
#define END_ATOMIC() __bis_SR_register(GIE)

void retrieveState();

void startFlash() {
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1MHz
  DCOCTL = CALDCO_1MHZ;
  FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator
  retrieveState();
}

void saveState() {
  char *segC,*segD;
  char i;
  long checksum;
  char *checksumPtr = (char*)(&checksum);
  char newData[sizeof(stateVec)];
  START_ATOMIC();

  // Copy globalState to a local variable - atomic operation. Also generate checksum.
  checksum = generateChecksum(&globalState);
  for(i = 0; i < sizeof(stateVec); ++i) {
    newData[i] = data[i];
  }

  segC = (char *)0x1040;
  segD = (char *)0x1000;

  // Clear lock bit
  FCTL3 = FWKEY;

  // First write to segment D
  // Erase
  FCTL1 = FWKEY | ERASE | EEI;
  *segD = 0;
  // Write data
  FCTL1 = FWKEY | WRT;
  for(i = 0; i < sizeof(stateVec); ++i) {
    *segD++ = newData[i];
  }

  // Checksum
  *segD++ = checksumPtr[0];
  *segD++ = checksumPtr[1];
  *segD++ = checksumPtr[2];
  *segD++ = checksumPtr[3];
  
  // Now write to segment C
  // Erase
  FCTL1 = FWKEY | ERASE | EEI;
  *segC = 0;
  // Write data
  FCTL1 = FWKEY | WRT;
  for(i = 0; i < sizeof(stateVec); ++i) {
    *segC++ = newData[i];
  }

  *segD++ = checksumPtr[0];
  *segD++ = checksumPtr[1];
  *segD++ = checksumPtr[2];
  *segD++ = checksumPtr[3];

  // Clear write bit
  FCTL1 = FWKEY;
  // Set lock bit
  FCTL3 = FWKEY | LOCK;
  END_ATOMIC();
}

void retrieveState() {
  char *segC,*segD;
  char newData[sizeof(stateVec)];
  long checksum;
  char *checksumPtr = (char *)(&checksum);
  char i;

  if(DEBUG_MODE) {
    newStateVec();
    saveState();
    return;
  }

  segC = (char *)0x1040;
  segD = (char *)0x1000;

  // Clear lock bit
  FCTL3 = FWKEY;

  // Read data from segment C first
  for(i = 0; i < sizeof(stateVec); ++i) {
    newData[i] = *segC++;
  }

  // Read checksum;
  checksumPtr[0] = *segC++;
  checksumPtr[1] = *segC++;
  checksumPtr[2] = *segC++;
  checksumPtr[3] = *segC++;

  // If it passes, return otherwise try segD
  if(checkChecksum(checksum, (stateVec*)newData)) {
    for(i = 0; i < sizeof(stateVec); ++i) {
      data[i] = newData[i];
    }
    // Set lock bit
    FCTL3 = FWKEY+LOCK;
    return;
  }

  else {
    // Read data from segment D (checksum check failed)
    for(i = 0; i < sizeof(stateVec); ++i) {
      newData[i] = *segD++;
    }

    // Read checksum;
    checksumPtr[0] = *segD++;
    checksumPtr[1] = *segD++;
    checksumPtr[2] = *segD++;
    checksumPtr[3] = *segD++;
    
    // If it passes, return, otherwise die
    if(checkChecksum(checksum, (stateVec*)newData)) {
      for(i = 0; i < sizeof(stateVec); ++i) {
	data[i] = newData[i];
      }
    }
    else {
      // Something's gone really wrong, go into safemode
      newStateVec();
      globalState.safemode = 1;
      saveState();
    }
  }
  // Set lock bit
  FCTL3 = FWKEY+LOCK;
}

void newStateVec() {
  char i;
  for(i = 0; i < sizeof(stateVec); ++i) {
    data[i] = 0;
  }
}

long generateChecksum(stateVec *state) {
  return
    state->currentStepOne^
    state->desiredStepOne^
    state->curSignalOne^

    state->currentStepTwo^
    state->desiredStepTwo^
    state->curSignalTwo^

    state->internalTime^
    state->externalTime^

    state->temperature^

    state->safemode^

    state->temperature;
}

char checkChecksum(long checksum, stateVec *state) {
  return (generateChecksum(state) == checksum);
}
