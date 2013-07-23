#include <msp430.h>
#include "serial.h"
#include "motors.h"
#include "parsing.h"
#include "timer.h"
#include "state.h"

#define N_BUF 2

// Globals
char outputBuffers[N_BUF][150];
char lengths[N_BUF];
char outSel = 0;
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
   HASP information comes in the format 0x1 0x30 +120 bytes + 0x30 + .
   Commands from ground never start with 0x3 or above 0x9 (7 possible), and are two bytes long.
*/
char messageStarted = 0;
unsigned char first;

void parseByte(char b) {
  gpsOut g;
  // First byte, with error checking
  if(!messageStarted || b == 0x1) {
    first = b;
    messageStarted = 1;
  }
  // Second byte
  else if(messageStarted == 1) {
    // GPS data
    if(first == 0x1 && b == 0x30) {
      messageStarted = 3;
    }
    // Command
    else if(b == first) {
      messageStarted = 0;
      doCommand(b);
    }
    // If valid start byte, assume out of frame error -- corrects itself in case of garbling
    else if(b <= 0x9) {
      first = b;
    }
    else {
      messageStarted = 0;
    }
  }
  // We're in GPS data
  else if(messageStarted == 3) {
    g = gpsParse(b);
    if(g.ended) {
      if(g.height >= 0) {
	globalState.height = g.height;
	doCommand(0);
      }
      globalState.externalTime = g.timestamp;
      resetGPS();
      messageStarted = 4;
    }
  }
  // We're in GPS tail
  else if(messageStarted == 4) {
    // Not in GPS tail? Data corruption or bytes dropped maybe.
    if(b != 0x3 && b != 0xD && b != 0xA) {
      messageStarted = 0;
    }
    if(b == 0xA) {
      messageStarted = 0;
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

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
  parseByte(UCA0RXBUF);
  __bic_SR_register_on_exit(LPM0_bits);
}

// Sends a message with format we specified.
char message [] = "0123456789abcdefghijklmnopqrstuvwxyzAB";
void sendLog(char command) {
  // command
  message[0]  = command;
  
  // internal time
  message[1]  = (char)((globalState.internalTime>>24)     );
  message[2]  = (char)((globalState.internalTime>>16)&0xFF);
  message[3]  = (char)((globalState.internalTime>> 8)&0xFF);
  message[4]  = (char)((globalState.internalTime     &0xFF));
  
  // external time
  message[5]  = (char)((globalState.externalTime>>24)     );
  message[6]  = (char)((globalState.externalTime>>16)&0xFF);
  message[7]  = (char)((globalState.externalTime>> 8)&0xFF);
  message[8]  = (char)((globalState.externalTime    )&0xFF);
 
  // height
  message[9]  = (char)((globalState.height>>24)     );
  message[10] = (char)((globalState.height>>16)&0xFF);
  message[11] = (char)((globalState.height>> 8)&0xFF);
  message[12] = (char)((globalState.height    )&0xFF);
  
  // temperature
  message[13] = (char)((globalState.temperature>>8)     );
  message[14] = (char)((globalState.temperature   )&0xFF);
  
  // checksum
  char checkbyte = 15;
  message[checkbyte] = 0;
  int i;
  for (i=0 ; i < checkbyte ; i++) {
    message[checkbyte] ^= message[i];
  }

  // reserved
  message[16] = 0xFF;
  message[17] = 0xFF;
  message[18] = 0xFF;
  message[19] = 0xFF;
  message[20] = 0xFF;
  message[21] = 0xFF;
  message[22] = 0xFF;
  message[23] = 0xFF;
  message[24] = 0xFF;
  message[25] = 0xFF;
  message[26] = 0xFF;
  message[27] = 0xFF;
  message[28] = 0xFF;
  message[29] = 0xFF;
  message[30] = 0xFF;
  message[31] = 0xFF;
  message[32] = 0xFF;

  // padding
  message[33] = 0xFF;
  message[34] = 0xFF;
  message[35] = 0xFF;
  message[36] = 0xFF;
  message[37] = 0xFF;
  
  serialSend(message,38);
}
