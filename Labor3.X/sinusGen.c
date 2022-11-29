#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>
#include "sinus.h"


void delay_us();
uint16_t readADC();
void initVoltageRefUnit();
void initADC();

uint8_t ind = 0;

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
    ind++;                                // increment
    ind = ind % 100;                         // mod 100
    uint8_t sinData = sinus[ind];
    DAC1CONbits.DACDAT = sinData;             // write back
}

void nextSinusASMOutput(){
    uint8_t numberSteps = 100;
    asm volatile(
        "addi $t0, %0, 1           \n\t"    // ind++
        "bne $t0, %2, 1f           \n\t"    // ind != 100 ? 1f : weiter
        "nop                       \n\t"
        "and $t0, $zero, $zero     \n\t"    // ind = 0
        "1:                        \n\t"
        "add %0, $t0, $zero         \n\t"    // write t0 back to %0
        "la $t1, %1                \n\t"    // address of sinus in t1
        "add $t1, $t1, $t0         \n\t"    // t1 + i -> &sinus[i]
        "lw $t0, 0($t1)            \n\t"    // t0 = sinus[i]
        "lw $t1, DAC1CON           \n\t"    // t1 hat DAC1CON
        "ins $t1, $t0, 16, 5       \n\t"    // DAC1CONbits.DACDAT = t0
        : "+r" (ind)
        : "m"(sinus), "r"(numberSteps)
        : "t0", "t1", "t2"
    );
}

void __ISR(_TIMER_1_VECTOR, IPL3SOFT) sinusHandler(void)
{
    nextSinusOutput();
    IFS0bits.T1IF = 0;
}