#include <msp430.h>
#include "serial.h"
#include "motors.h"
#include "parsing.h"
#include "timer.h"

#define N_BUF 2

// Globals
char serialRecvFlag;
char serialRecvType;
char inputBuffers[N_BUF][30];
char outputBuffers[N_BUF][150];
char lengths[N_BUF];
char inpSel = 0;
char outSel = 0;
char length = 0;
char *serialInpBuf;
char i,o;

static long int height = 0;

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

  // Set FORCEON to OUTPUT
  P3DIR |= (1<<2);
}

/*
  FORCEON 3.2
*/
void sleepMode() {
  // Set FORCEON to LOW
  P3OUT |= (1<<2);
}

void wakeUp() {
  // Set FORCEON to HIGH
  P3OUT |= (1<<2);
}

// Send a string through serial.
char newMsgs = 0;
void serialSend(char *str, char length) {
  char oS;
  char *buf;

  ++newMsgs;
  // Copy str to buffer
  oS = (outSel+1)%N_BUF;
  buf = outputBuffers[oS];
  lengths[oS] = length;
  
  while((buf - outputBuffers[oS]) != length) {
    *(buf++) = *(str++);
  }
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
  gpsOut g;
  /*
    We might get a ground command/gps Start and think we are in a GPS string.
    Could happen in case of lost byte in GPS string.
    All commands/GPS string are started by bytes in [0x01,0x09]
    These never appear in the GPS string (except byte 0x03), so we just reset it
      in case we find them.
   */
  if(!messageType && b != 0x03 && b <= 0x09) {
    resetGPS();
    messageStarted = 0;
  }

  if(!messageStarted) {
    messageStarted = 1;
    i = 0;
    inpSel = (inpSel+1)%N_BUF;
    messageType = b-1;
  }
  if(!messageType) {
    g = gpsParse(b);
    if(g.ended) {
      if(g.height >= 0) {
	height = g.height;
	sendLog(0);
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
  if(o == lengths[outSel]) {
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
  __bic_SR_register_on_exit(LPM0_bits);
}

// Sends a message with format we specified.
char message [] = "1234567890";
void sendLog(char command) {
  // MS since startup
  message[0] = (char)(timerMS>>24);
  message[1] = (char)((timerMS>>16)&0xFF);
  message[2] = (char)((timerMS>>8)&0xFF);
  message[3] = (char)(timerMS&0xFF);
  // Height
  message[4] = (char)(height>>24);
  message[5] = (char)((height>>16)&0xFF);
  message[6] = (char)((height>>8)&0xFF);
  message[7] = (char)(height&0xFF);
  message[8] = command;
  // Parity byte checksum
  message[9] =
    message[0]^message[1]^message[2]^message[3]^message[4]
    ^message[5]^message[6]^message[7]^message[8];

  serialSend(message,10);
}
