#include <msp430.h>
#include "serial.h"

#define N_BUF 3

// Globals
char serialRecvFlag;
char serialRecvType;
char inputBuffers[N_BUF][150];
char outputBuffers[N_BUF][30];
char inpSel = 0;
char outSel = 0;
char *serialInpBuf;
char i,o;

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
  // Copy str to buffer
  oS = (outSel+1)%N_BUF;
  buf = outputBuffers[oS];
  while(*str) {
    *(buf++) = *(str++);
  }
  *(buf) = 0;
  if(!newMsgs) {
    // Wait for TX Buffer to be ready.
    while (!(IFG2&UCA0TXIFG));

    outSel = oS;
    o = 0;

    // Enable TX interrupt and send first character
    IE2 |= UCA0TXIE;
    UCA0TXBUF = outputBuffers[outSel][o++];
  }
  ++newMsgs;
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
#define HASP_LEN 122
#define GROUND_LEN 2
void parseByte(char b) {
  if(!messageStarted) {
    messageStarted = 1;
    i = 0;
    inpSel = (inpSel+1)%N_BUF;
    messageType = b-1;
  }
  inputBuffers[inpSel][i++] = b;
  if((messageType && i == GROUND_LEN) || i == HASP_LEN) {
    messageStarted = 0;
    serialInpBuf = inputBuffers[inpSel];
    serialRecvFlag = 1;
    serialRecvType = messageType;
  }
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void) {
  UCA0TXBUF = outputBuffers[outSel][o++];
  if(outputBuffers[outSel][o-1] == 0) {
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
