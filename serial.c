#include <msp430.h>

#define N_BUF 5

// Globals
char serialRecvFlag;
char inputBuffers[5][256];
char outputBuffers[5][256];
char inpSel = 0;
char outSel = 0;
char *serialInpBuf;
char i,o;

void serialStart() {
  if (CALBC1_1MHZ==0xFF) {// If calibration constant erased
    while(1);                               // do not load, trap CPU!!
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;
  P3SEL = 0x30;                             // P3.4,5 = USCI_A0 TXD/RXD
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 0x3;                            // 1MHz 1200
  UCA0BR1 = 0x41;                           // 1MHz 1200
  UCA0MCTL = 0x92;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

// Send a string through serial.
void serialSend(char *str) {
  // Wait for TX Buffer to be ready.
  while (!(IFG2&UCA0TXIFG));

  // Copy str to buffer
  outSel = (outSel+1)%N_BUF;
  o = 0;
  char *buf = outputBuffers[outSel];
  while(*str) {
    *(buf++) = *(str++);
  }
  *(buf) = NULL;

  // Enable TX interrupt and send first character
  IE2 |= UCA0TXIE;
  UCA0TXBUF = outputBuffers[outSel][o++];
}

// Parses the next byte received.
// It keeps internal state through global variables.
void parseByte(char b) {
  return;
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void) {
  UCA0TXBUF = outputBuffers[outSel][o++];
  if(outputBuffers[outSel][o] != NULL) {
    IE2 &= ~UCA0TXIE;
  }
}

//  Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
  parseByte(UCA0RXBUF);
}
