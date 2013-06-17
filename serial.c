#include <msp430.h>
#include "serial.h"
#include "motors.h"
#include "parsing.h"

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

void sleepMode();
void wakeUp();
void parseByte(char);

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
  sleepMode();
  IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt

  // Set EN and FORCEON to OUTPUT
  P3DIR |= (1<<6);
  P2DIR |= (1<<3);
  // Set EN to LOW
  P2OUT &= ~(1<<3);
}

/*
  FORCEON 3.6
  ENABLE 2.3
*/
void sleepMode() {
  // Set ENABLE to LOW -- already done
  // Set FORCEON to LOW
  P3OUT &= ~(1<<6);
}

void wakeUp() {
  // Set FORCEON to HIGH
  P3OUT |= (1<<6);
  // Set EN to LOW -- already done
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
    wakeUp();
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
char gpsSend[] = ";;;;;\n";

void parseByte(char b) {
  long int height;
  gpsOut g;
  if(!messageStarted) {
    messageStarted = 1;
    i = 0;
    inpSel = (inpSel+1)%N_BUF;
    messageType = b-1;
  }
  if(!messageType) {
    g = gpsParse(b);
    if(g.ended) {
      if(g.height < 0) {
	serialSend("BAD\n");
	return;
      }
      else {
	height = g.height;
	gpsSend[4] = (height%10) + '0';
	height /= 10;
	gpsSend[3] = (height%10) + '0';
	height /= 10;
	gpsSend[2] = (height%10) + '0';
	height /= 10;
	gpsSend[1] = (height%10) + '0';
	height /= 10;
	gpsSend[0] = (height%10) + '0';
	serialSend(gpsSend);
	messageStarted = 0;
      }
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

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void) {
  // Wait for TX Buffer to be ready.
  UCA0TXBUF = outputBuffers[outSel][o++];
  if(outputBuffers[outSel][o] == 0) {
    --newMsgs;
    if(!newMsgs) {
      sleepMode();
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
