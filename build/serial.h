#ifndef SERIAL_H
#define SERIAL_H

#define LEN_OUT_BUF 256
#define LEN_INP_BUF 150

#define FORCEON_DIR P2DIR
#define FORCEON_OUT P2OUT
#define FORCEON_PIN (0b1)

void serialStart();
void serialSend(char *,int);
void sendLog(char);

#endif