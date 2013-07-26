#include <msp430.h>
#include "hard.h"
#include "state.h"

void startHard() {
  turnHVDC1Off();
  turnHVDC2Off();
  turnHeater1Off();
  turnHeater2On();
  HVDC1DIR |= HVDC1PIN;
  HVDC2DIR |= HVDC2PIN;
  HEATER1DIR |= HEATER1PIN;
  HEATER2DIR |= HEATER2PIN;
}

void turnHVDC1On() {
  HVDC1OUT |= HVDC1PIN;
  globalState.HVDCOne = 1;
}

void turnHVDC1Off() {
  HVDC1OUT &= ~(HVDC1PIN);
  globalState.HVDCOne = 0;
}

void turnHVDC2On() {
  HVDC2OUT |= HVDC2PIN;
  globalState.HVDCTwo = 1;
}

void turnHVDC2Off() {
  HVDC2OUT &= ~HVDC2PIN;
  globalState.HVDCTwo = 0;
}

void turnHeater1On() {
  HEATER1OUT |= HEATER1PIN;
  globalState.heaterOne = 1;
}

void turnHeater1Off() {
  HEATER1OUT &= ~HEATER1PIN;
  globalState.heaterOne = 0;
}

void turnHeater2On() {
  HEATER2OUT |= HEATER2PIN;
  globalState.heaterTwo = 1;
}

void turnHeater2Off() {
  HEATER2OUT &= ~HEATER2PIN;
  globalState.heaterTwo = 0;
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
    globalState.temperature = AVERAGE_TEMP(temperature_big);
    temperature_big = 0;
    currentMeasure = 0;
  }
  adcStart();
}
