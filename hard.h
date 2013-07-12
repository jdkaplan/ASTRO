#ifndef HARD_H
#define HARD_H

void startHard();

#define HDVC1DIR P3DIR
#define HDVC1OUT P3OUT
#define HDVC1PIN (1<<3)
void turnHDVC1On();
void turnHDVC1Off();

#define HDVC2DIR P3DIR
#define HDVC2OUT P3OUT
#define HDVC2PIN (1<<0)
void turnHDVC2On();
void turnHDVC2Off();

#define HEATERDIR P3DIR
#define HEATEROUT P3OUT
#define HEATERPIN (1<<1)
void turnHeaterOn();
void turnHeaterOff();

#define TEMP_SHIFT 5
#define N_MEASURE (1<<TEMP_SHIFT)
extern int temperature;
void adcStart();

#endif
