#ifndef SERIAL_H
#define SERIAL_H
void serialStart();
void serialSend(char *);
extern char serialRecvFlag;
extern char *serialInpBuf;
#endif
