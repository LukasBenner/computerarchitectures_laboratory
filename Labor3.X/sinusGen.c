#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>
#include "sinus.h"

typedef unsigned char u8;

void delay_us();
uint16_t readADC();
void initVoltageRefUnit();
void initADC();

void sinusGen(){
    initVoltageRefUnit();
    initADC();
   
    T1CON = 0x0;                // Stops the Timer1 and resets the control register
    TMR1 = 0x0;                 // Clear timer register
    IFS0bits.T1IF = 0;          // Clear int status reg
    
    IPC4bits.T1IP = 3;          // Priority level 3
    IPC4bits.T1IS = 1;          // Subpriority level 1
    IEC0bits.T1IE = 1;          // Enable timer interrupts
    
    T1CONbits.TCS = 0;          // Internal PB Clock   24 MHz / 16kHz = 1500
    T1CONbits.TCKPS = 0b00;     // prescaler to 1
    PR1 = 1500;                 // Timer to 1500
    // Timer is set to 1Hz
    T1CONbits.ON = 1;           // Start Timer
    
    while(1){
        uint16_t adcVal = readADC();
        // 24 MHz / (200Hz × 100) = 1200
        // offset 1200   steigung von 960 / 4096 = 0.234
        uint16_t data = 1200 - (adcVal * 234) / 1000;
        PR1 = data;
    }
}

void nextSinusOutput(){
    uint8_t i = DAC1CONbits.DACDAT;     // load
    i++;                                // increment
    i = i % 100;                         // mod 32
    u8 sinData = sinus[i];
    DAC1CONbits.DACDAT = sinData;             // write back
}

void __ISR(_TIMER_1_VECTOR, IPL3SOFT) sinusHandler(void)
{
    nextSinusOutput();
    IFS0bits.T1IF = 0;
}