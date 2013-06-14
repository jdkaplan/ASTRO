#ifndef SERIAL_H
#define SERIAL_H
void serialStart();
void serialSend(char *);
extern char serialRecvFlag;
extern char serialRecvType;
extern char *serialInpBuf;
#endif
