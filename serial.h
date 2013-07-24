#ifndef SERIAL_H
#define SERIAL_H

#define N_OUT_BUF 5
#define LEN_OUT_BUF 50
#define LEN_INP_BUF 150

#define FORCEON_DIR P2DIR
#define FORCEON_OUT P2OUT
#define FORCEON_PIN (0b1)

void serialStart();
void serialSend(char *,char);
void sendLog(char);

#endif
