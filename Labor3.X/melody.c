#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>
#include "notes.h"


void delay_us();
void initVoltageRefUnit();

typedef struct
{
    uint32_t freq;
    uint32_t duration;
} Note;

Note melodyStruct[] = {
    {NOTE_C6, 4},
    {NOTE_G5, 8},
    {NOTE_G5, 8},
    {NOTE_A5, 4},
    {NOTE_G5, 4},
    {0, 4},
    {NOTE_B5, 4},
    {NOTE_C6, 4}
};

uint8_t melodyLength = 27;

void melody(){
    initVoltageRefUnit();
   
    T1CON = 0x0;                // Stops the Timer1 and resets the control register
    TMR1 = 0x0;                 // Clear timer register
    IFS0bits.T1IF = 0;          // Clear int status reg
    
    IPC4bits.T1IP = 3;          // Priority level 3
    IPC4bits.T1IS = 1;          // Subpriority level 1
    IEC0bits.T1IE = 1;          // Enable timer interrupts
    
    T1CONbits.TCS = 0;          // Timer source to PBCLK
    T1CONbits.TCKPS = 0b00;     // prescaler to 1
    PR1 = 545;                   // Timer to 545 -> A4
    // Timer is set to 1Hz
    T1CONbits.ON = 1;           // Start Timer
    
    while(1){
        for(int i = 0; i < melodyLength; i++){
            Note note = melodyStruct[i];
            uint32_t freq = note.freq;
            uint32_t duration = note.duration;
            uint32_t timerInterval;
            if(freq == 0){
                T1CONbits.ON = 0;
                DAC1CONbits.DACDAT = 0;
            }
            else{
                T1CONbits.ON = 1;
                timerInterval = 24000000 / (freq * 100);   // 24MHz  / (3951Hz × 100)
            }
            PR1 = timerInterval;
            uint32_t durationInUs = 1000000 / duration;
            delay_us(durationInUs);
            
        }
        T1CONbits.ON = 0;           // StopTimer
        delay_us(2000000);
        T1CONbits.ON = 1;           // Start Timer
    }
    
}

void nextSinusOutput();

void __ISR(_TIMER_1_VECTOR, IPL3SOFT) melodyHandler(void)
{
    nextSinusOutput();
    IFS0bits.T1IF = 0;
}