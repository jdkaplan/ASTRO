#include <msp430.h>
#include "hard.h"
#include "state.h"

void startHard() {
  turnHDVC1Off();
  turnHDVC2Off();
  turnHeater1Off();
  turnHeater2On();
  HDVC1DIR |= HDVC1PIN;
  HDVC2DIR |= HDVC2PIN;
  HEATER1DIR |= HEATER1PIN;
  HEATER2DIR |= HEATER2PIN;
}

void turnHDVC1On() {
  HDVC1OUT |= HDVC1PIN;
}

void turnHDVC1Off() {
  HDVC1OUT &= ~(HDVC1PIN);
}

void turnHDVC2On() {
  HDVC2OUT |= HDVC2PIN;
}

void turnHDVC2Off() {
  HDVC2OUT &= ~HDVC2PIN;
}

void turnHeater1On() {
  HEATER1OUT |= HEATER1PIN;
}

void turnHeater1Off() {
  HEATER1OUT &= ~HEATER1PIN;
}

void turnHeater2Off() {
  HEATER2OUT &= ~HEATER2PIN;
}

void turnHeater2On() {
  HEATER2OUT |= HEATER2PIN;
}

int temperature = 0;

void adcStart() {
  ADC10CTL0 &= ~ENC;// Disable ADC
  ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ADC10IE; // 64 clock ticks, ADC On, enable ADC interrupt
  ADC10CTL1 = ADC10SSEL_3 | INCH_2; // Set A0, SMCLK
  ADC10CTL0 |= ENC + ADC10SC; // Enable and start conversion
}

char currentMeasure = 0;
long int temperature_big = 0;
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void) {
  temperature_big += ADC10MEM; // Saves measured value.
  ++currentMeasure;
  if(currentMeasure == N_MEASURE) {
    globalState.temperature = temperature_big>>TEMP_SHIFT;
    temperature_big = 0;
    currentMeasure = 0;
  }
  adcStart();
}
