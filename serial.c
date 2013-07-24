#include <msp430.h>
#include "serial.h"
#include "motors.h"
#include "parsing.h"
#include "timer.h"
#include "state.h"

#define START_ATOMIC() __bic_SR_register(GIE)
#define END_ATOMIC() __bis_SR_register(GIE)

// Globals
volatile char outputBuffers[N_OUT_BUF][LEN_OUT_BUF];
volatile unsigned char inputBuffer[LEN_INP_BUF];
volatile int lengths[N_OUT_BUF];
volatile int outSel = 0,inpSel = 0,inputTop = 0;
volatile int currentOutByte;

void sleepMode();
void wakeUp();
void parseByte();

void serialStart() {
  if (CALBC1_1MHZ==0xFF) {// If calibration constant erased
    while(1); // do not load, trap CPU!!
  }
  DCOCTL = 0; // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ; // Set DCO
  DCOCTL = CALDCO_1MHZ;
  P3SEL = 0x30; // P3.4,5 = USCI_A0 TXD/RXD
  // 1MHz SMCLK
  UCA0CTL1 |= UCSSEL_2; // SMCLK
  UCA0BR0 = 0x41; // 1MHz 1200
  UCA0BR1 = 0x3; // 1MHz 1200
  UCA0MCTL = 0x92; // Modulation UCBRSx = 1
  // 32khz ACLK
  /*UCA0CTL1 |= UCSSEL_1;
  UCA0BR0 = 0x1b;
  UCA0BR1 = 0x0;
  UCA0MCTL = 0x12;*/
  UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
  sleepMode();
  IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt

  // Set FORCEON to OUTPUT
  FORCEON_DIR |= FORCEON_PIN;
}

/*
  FORCEON 2.1
*/
void sleepMode() {
  // Set FORCEON to LOW
  FORCEON_OUT &= ~FORCEON_PIN;
}

void wakeUp() {
  // Set FORCEON to HIGH
  FORCEON_OUT |= FORCEON_PIN;
}

// Send a string through serial.
// Always run from main
// Concurrency safe
char newMsgs = 0;
void serialSend(char *str, char length) {
  char oS;
  volatile char *buf;
  char startMsgCount;

  START_ATOMIC();
  startMsgCount = ++newMsgs;
  // Copy str to buffer
  oS = (outSel+1)%N_OUT_BUF;
  buf = outputBuffers[oS];
  lengths[oS] = length;
  END_ATOMIC();

  // Note: did not make copying string to buffer atomic because if there are concurrency
  //  issues, it means we are overwriting a buffer in use, which indicates bigger problems
  //  (and a need for more buffers)
  while((buf - outputBuffers[oS]) != length) {
    *(buf++) = *(str++);
  }
  // Not atomic because these variables MUST NOT be touched if newMsgs is 1 here.
  if(startMsgCount == 1) {
    // Wait for TX Buffer to be ready.
    while (!(IFG2&UCA0TXIFG));

    outSel = oS;
    currentOutByte = 0;

    // Enable TX interrupt and send first character
    wakeUp();
    UCA0TXBUF = outputBuffers[outSel][currentOutByte++];
    IE2 |= UCA0TXIE;
  }
}


// Parses the next byte received.
// It keeps internal state through global variables.

/* Notes about the parsing:
   HASP information comes in the format 0x1 0x30 +120 bytes + 0x30 + .
   Commands from ground never start with 0x3 or above 0x9 (7 possible), and are two bytes long.
*/
// Always called from main
char messageStarted = 0;
unsigned char first;

void parseByte() {
  //P1OUT |= 0x1;
  unsigned char b;
  START_ATOMIC();
  b = inputBuffer[inpSel];
  inpSel = (inpSel+1)%LEN_INP_BUF;
  END_ATOMIC();

  if((b <= 0x9 && messageStarted != 1) || messageStarted == 0) {
    // valid start bytes are 0x0 thru 0x9 except for 0x3
    if (b <= 0x09 && b != 0x03) {
      messageStarted = 1;
      first = b;
    }
    // In case of corrupt data/dropped bytes
    else {
      messageStarted = 0;
    }
  }
  
  // expecting second byte of message
  else if (messageStarted == 1) {
    // expecting HASP data, 0x30 is second byte of HASP string
    if (first == 0x01 && b == 0x30) {
      messageStarted = 2;
      resetGPS();
    }
    // expecting command, commands are pairs of identical bytes
    else if (b == first) {
      messageStarted = 0;
      doCommand(b);
    }
    // If valid start byte, assume out of frame error -- corrects itself in case of garbling
    else if (b <= 0x9 && b != 0x03) {
      first = b;
    }
    // invalid second byte
    else {
      messageStarted = 0;
    }
  }

  // parsing GPS stuff
  else if (messageStarted == 2) {
    // run it through the gpsParse state machine
    gpsOut g = gpsParse(b);
    
    // b was the last byte expected by gpsParse
    if (g.ended) {
      // positive altitude!
      if (g.height >= 0) {
        // store it!
        globalState.height = g.height;
      }
      // because I am a clock
      globalState.externalTime = g.timestamp;
      // pong
      doCommand(0);
      messageStarted = 0;
    }
    
    // gps string hasn't ended yet
    else {
      // pass
    }
  }

  else {
    // How the fuck did you get here?
    // Probably GPS tail :P or corrupted dick
  }

  //P1OUT &= ~0x3;
  //P1OUT |= messageStarted;
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void) {
  // Wait for TX Buffer to be ready.
  UCA0TXBUF = outputBuffers[outSel][currentOutByte++];
  if(currentOutByte == lengths[outSel]) {
    --newMsgs;
    if(!newMsgs) {
      sleepMode();
      IE2 &= ~UCA0TXIE;
    }
    else {
      currentOutByte = 0;
      outSel = (outSel+1)%N_OUT_BUF;
    }
  }
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
  inputBuffer[inputTop] = UCA0RXBUF;
  inputTop = (inputTop+1)%LEN_INP_BUF;
  doAction(&parseByte);
  __bic_SR_register_on_exit(LPM0_bits);
}

// Sends a message with format we specified.
char message [] = "0123456789abcdefghijklmnopqrstuvwxyzAB";
void sendLog(char command) {
  // command
  message[0]  = command;
  
  // internal time
  START_ATOMIC();
  message[1]  = (char)((globalState.internalTime>>24)     );
  message[2]  = (char)((globalState.internalTime>>16)&0xFF);
  message[3]  = (char)((globalState.internalTime>> 8)&0xFF);
  message[4]  = (char)((globalState.internalTime    )&0xFF);
  END_ATOMIC();
  
  // external time
  START_ATOMIC();
  message[5]  = (char)((globalState.externalTime>>24)     );
  message[6]  = (char)((globalState.externalTime>>16)&0xFF);
  message[7]  = (char)((globalState.externalTime>> 8)&0xFF);
  message[8]  = (char)((globalState.externalTime    )&0xFF);
  END_ATOMIC();
 
  // height
  START_ATOMIC();
  message[9]  = (char)((globalState.height>>24)     );
  message[10] = (char)((globalState.height>>16)&0xFF);
  message[11] = (char)((globalState.height>> 8)&0xFF);
  message[12] = (char)((globalState.height    )&0xFF);
  END_ATOMIC();
  
  // temperature
  START_ATOMIC();
  message[13] = (char)((globalState.temperature>>8)     );
  message[14] = (char)((globalState.temperature   )&0xFF);
  END_ATOMIC();
  
  // checksum
  char checkbyte = 15;
  message[checkbyte] = 0;
  char i;
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
