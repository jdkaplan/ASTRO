#ifndef HARD_H
#define HARD_H

void startHard();

#define HDVC1DIR P4DIR
#define HDVC1OUT P4OUT
#define HDVC1PIN (1<<3)
void turnHDVC1On();
void turnHDVC1Off();

#define HDVC2DIR P4DIR
#define HDVC2OUT P4OUT
#define HDVC2PIN (1<<4)
void turnHDVC2On();
void turnHDVC2Off();

#define HEATER1DIR P4DIR
#define HEATER1OUT P4OUT
#define HEATER1PIN (1<<5)
void turnHeater1On();
void turnHeater1Off();

#define HEATER2DIR P4DIR
#define HEATER2OUT P4OUT
#define HEATER2PIN (1<<5)
void turnHeater2On();
void turnHeater2Off();

#define TEMP_SHIFT 5
#define N_MEASURE (1<<TEMP_SHIFT)
void adcStart();

#endif
