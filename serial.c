#include <msp430.h>
#include "serial.h"

#define N_BUF 2

// Globals
char serialRecvFlag;
char serialRecvType;
char inputBuffers[N_BUF][30];
char outputBuffers[N_BUF][150];
char inpSel = 0;
char outSel = 0;
char *serialInpBuf;
char i,o;

#define GROUND_LEN 2
#define HASP_LEN 122

typedef struct {
  long int height;
  char ended;
} gpsOut;

void parseByte(char);
gpsOut gpsParse(char);
void doCommand(char);

void serialStart() {
  if (CALBC1_1MHZ==0xFF) {// If calibration constant erased
    while(1); // do not load, trap CPU!!
  }
  DCOCTL = 0; // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ; // Set DCO
  DCOCTL = CALDCO_1MHZ;
  P3SEL = 0x30; // P3.4,5 = USCI_A0 TXD/RXD
  UCA0CTL1 |= UCSSEL_2; // SMCLK
  UCA0BR0 = 0x41; // 1MHz 1200
  UCA0BR1 = 0x3; // 1MHz 1200
  UCA0MCTL = 0x92; // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
  IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt
}

// Send a string through serial.
char newMsgs = 0;
void serialSend(char *str) {
  char oS;
  char *buf;

  ++newMsgs;
  // Copy str to buffer
  oS = (outSel+1)%N_BUF;
  buf = outputBuffers[oS];
  while(*str) {
    *(buf++) = *(str++);
  }
  *(buf) = 0;
  if(newMsgs == 1) {
    // Wait for TX Buffer to be ready.
    while (!(IFG2&UCA0TXIFG));

    outSel = oS;
    o = 0;

    // Enable TX interrupt and send first character
    UCA0TXBUF = outputBuffers[outSel][o++];
    IE2 |= UCA0TXIE;
  }
}


// Parses the next byte received.
// It keeps internal state through global variables.

/* Notes about the parsing:
   HASP information comes in the format 0x1 0x3 +120 bytes.
   Commands from ground never start with 0x1, and are two bytes long.
   Have a flag variable on whether we already started receiving a message, and a type variable.
   messageType is 0 for messages from HASP and 1 for messages from ground.
 */
char messageStarted = 0;
char messageType = 0;
char gpsSend[] = "    \n";

void parseByte(char b) {
  gpsOut g;
  if(!messageStarted) {
    messageStarted = 1;
    i = 0;
    inpSel = (inpSel+1)%N_BUF;
    messageType = b-1;
  }
  if(messageType) {
    g = gpsParse(b);
    if(g.ended) {
      gpsSend[0] = g.height>>(3*8);
      gpsSend[1] = (g.height>>(2*8))&0xF;
      gpsSend[2] = (g.height>>8)&0xFF;
      gpsSend[3] = g.height&0xFF;
      serialSend(gpsSend);
      messageStarted = 0;
    }
    ++i;
  }
  else {
    inputBuffers[inpSel][i++] = b;
    if(i == GROUND_LEN) {
      messageStarted = 0;
      if(inputBuffers[inpSel][0] == inputBuffers[inpSel][1]) {
	doCommand(inputBuffers[inpSel][0]);
      }
    }
  }
}

// Gets byte by byte the GPS string. Parses it, and maintains a state.
// When finished, it should return the reported height and flag saying the string
//   ended, in the gpsOut format.
// While not finished, it should return the ended flag as false (just return {0,0}).
// Note: global variable i contains the current byte's index (DO NOT CHANGE IT!).
// Note: HASP_LEN is the length of the string, including the two initial characters.

gpsOut gpsParse(char b) {
  gpsOut res = {0,0};
  return res;
}

// Does the command indicated by the command byte.
void doCommand(char comm) {
  serialSend("A command was received!\n");
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void) {
  // Wait for TX Buffer to be ready.
  UCA0TXBUF = outputBuffers[outSel][o++];
  if(outputBuffers[outSel][o] == 0) {
    --newMsgs;
    if(!newMsgs) {
      IE2 &= ~UCA0TXIE;
    }
    else {
      o = 0;
      outSel = (outSel+1)%N_BUF;
    }
  }
}

//  Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
  parseByte(UCA0RXBUF);
}
