#ifndef HARD_H
#define HARD_H

void startHard();

#define HVDC1DIR P4DIR
#define HVDC1OUT P4OUT
#define HVDC1PIN (1<<3)
void turnHVDC1On();
void turnHVDC1Off();

#define HVDC2DIR P4DIR
#define HVDC2OUT P4OUT
#define HVDC2PIN (1<<4)
void turnHVDC2On();
void turnHVDC2Off();

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
#define AVERAGE_TEMP(T)  (T>>TEMP_SHIFT)
#define N_MEASURE (1<<TEMP_SHIFT)
void adcStart();

#endif
