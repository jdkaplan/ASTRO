; --COPYRIGHT--,BSD_EX
;  Copyright (c) 2012, Texas Instruments Incorporated
;  All rights reserved.
; 
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions
;  are met:
; 
;  *  Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
; 
;  *  Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
; 
;  *  Neither the name of Texas Instruments Incorporated nor the names of
;     its contributors may be used to endorse or promote products derived
;     from this software without specific prior written permission.
; 
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
;  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
;  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
;  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
;  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
;  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
;  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
;  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
;  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 
; ******************************************************************************
;  
;                        MSP430 CODE EXAMPLE DISCLAIMER
; 
;  MSP430 code examples are self-contained low-level programs that typically
;  demonstrate a single peripheral function or device feature in a highly
;  concise manner. For this the code may rely on the device's power-on default
;  register values and settings such as the clock configuration and care must
;  be taken when combining code from several examples to avoid potential side
;  effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
;  for an API functional library-approach to peripheral configuration.
; 
; --/COPYRIGHT--
;*******************************************************************************
;   MSP430F22x4 Demo - P1.2 Interrupt from LPM4 with Internal Pull-up Resistor
;
;   Description: A hi/low transition on P1.2 will trigger P1_ISR which,
;   toggles P1.0. Normal mode is LPM4 ~ 0.1uA. LPM4 current can be measured
;   with the LED removed, all unused Px.x configured as output or inputs
;   pulled high or low, and ensure the P1.2 interrupt input does not float.
;   Internal pullup enabled on P1.2.
;   ACLK = n/a, MCLK = SMCLK = default DCO
;
;                MSP430F22x4
;             -----------------
;         /|\|              XIN|-
;          | |                 |
;          --|RST          XOUT|-
;            |                 |
;            |     /|\         |
;            |      |          |
;       --o->| P1.2--      P1.0|-->LED
;       
;
;  P.Thanigai / W. Goh
;  Texas Instruments Inc.
;  November 2008
;  Built with Code Composer Essentials Version: 3.2.2
;*******************************************************************************
 .cdecls C,LIST,  "msp430.h"                            
;------------------------------------------------------------------------------
            .text                           ; Program reset
;------------------------------------------------------------------------------
RESET       mov.w   #300h,SP                ; Initialize stack pointer
StopWDT     mov.w   #WDTPW+WDTHOLD,&WDTCTL  ; Stop WDT
SetupP1     mov.b   #001h,&P1DIR            ; P1.0 output, else input
            mov.b   #004h,&P1OUT            ; P1.2 pullup
            bis.b   #004h,&P1REN            ; P1.2 pullup
            bis.b   #004h,&P1IE             ; P1.2 Interrupt enabled
            bis.b   #004h,&P1IES            ; P1.2 hi/low edge
            bic.b   #004h,&P1IFG            ; P1.2 IFG Cleared
                                            ;
Mainloop    bis.w   #LPM4+GIE,SR            ; LPM4, enable interrupts
            nop                             ; Required only for debugger
                                            ;
;-------------------------------------------------------------------------------
P1_ISR;     Toggle P1.0 Output
;-------------------------------------------------------------------------------
            xor.b   #001h,&P1OUT            ; P1.0 = toggle
            bic.b   #004h,&P1IFG            ; P1.2 IFG Cleared
            reti                            ; Return from ISR
                                            ;
;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET                   ;
            .sect   ".int02"                ; P1.x Vector
            .short  P1_ISR                  ;
            .end
