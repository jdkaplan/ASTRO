#include <msp430.h>
#include "hard.h"

void startHard() {
  turnHDVC1Off();
  turnHDVC2Off();
  turnHeaterOff();
  HDVC1DIR |= HDVC1PIN;
  HDVC2DIR |= HDVC2PIN;
  HEATERDIR |= HEATERPIN;
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

void turnHeaterOn() {
  HEATEROUT |= HEATERPIN;
}

void turnHeaterOff() {
  HEATEROUT &= ~HEATERPIN;
}

int temperature;

void adcStart() {
  ADC10CTL0 &= ~ENC;// Disable ADC
  ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ADC10IE | MSC; // 64 clock ticks, ADC On, enable ADC interrupt
  ADC10CTL1 = ADC10SSEL_3 | INCH_0; // Set A0, SMCLK
  ADC10CTL0 |= ENC + ADC10SC; // Enable and start conversion
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
  temperature = ADC10MEM;// Saves measured value.
}
