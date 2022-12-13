#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <sys/attribs.h>

typedef unsigned char u8;
typedef unsigned int u32;

void writeLCD (char* str, u32 len);
void delay_us(unsigned int us);

#define BUFFER_LENGTH 4
u32 distance;
u8 bufferIndex = 0;
u32 distanceBuffer[BUFFER_LENGTH];

void initInputCapture(){
    TRISAbits.TRISA9 = 1;   //RA9 as input
    
    CCP2CON1 = 0;
    CCP2CON2 = 0;
    CCP2CON1bits.CCSEL = 1; // Input capture mode
    CCP2CON1bits.CLKSEL = 0b000; // Set the clock source (Internal Clock ~ 24MHz)
    CCP2CON1bits.T32 = 0;   // 16-bit time base for timer
    CCP2CON1bits.MOD = 0b0011; // Every Rise/Fall (16-bit capture)
    CCP2CON2bits.ICS = 0b000;   // ICx Pin
    CCP2CON1bits.OPS = 0b0001;       //Interrupt every 2nd input capture event
    CCP2CON1bits.TMRPS = 0b11;     // Set the clock prescaler (1:64)
    CCP2CON1bits.SYNC = 0b00000; // Select Sync/Trigger source (Self-sync)
    RPINR2bits.ICM2R = 0b11000;   //ICM2 = RP24 = RA9
    
    IPC19bits.CCP2IP = 3;
    IPC19bits.CCP2IS = 1;
    IEC2bits.CCP2IE = 1;
    IFS2bits.CCP2IF = 0;
    
    CCP2CON1bits.ON = 1;        // Enable CCP/input capture
}

void initOutputCompare(){
    TRISAbits.TRISA12 = 0;  // set A12 to output
    
    CCP1CON1bits.CCSEL = 0; // Set MCCP operating mode (OC mode)
    CCP1CON1bits.T32 = 0; // Set timebase width (16-bit)
    CCP1CON1bits.MOD = 0b0101; // Set mode (Buffered Dual-Compare/PWM mode)
    CCP1CON1bits.TMRSYNC = 0; // Set timebase synchronization (not Synchronized)
    CCP1CON1bits.CLKSEL = 0b000; // Set the clock source (Internal Clock ~ 24MHz)
    CCP1CON1bits.TMRPS = 0b10; // Set the clock prescaler (1:16)
    CCP1CON1bits.TRIGEN = 0; // Set Sync/Triggered mode (Synchronous)
    CCP1CON1bits.SYNC = 0b00000; // Select Sync/Trigger source (Self-sync)
    //Configure MCCP output for PWM signal
    CCP1CON2bits.OCAEN = 1; // Enable desired output signals (OC1A)
    CCP1CON3bits.OUTM = 0b000; // Set advanced output modes (Standard output)
    CCP1CON3bits.POLACE = 0; // Configure output polarity (Active High)
    CCP1TMRbits.TMRL = 0x0000; // Initialize timer prior to enable module.
    CCP1PRbits.PRL = 0xFFFF; // Configure timebase period
    CCP1RA = 0x0000; // Set the rising edge compare value
    CCP1RB = 0xF; // Set the falling edge compare value to 10us
    CCP1CON1bits.ON = 1; // Turn on MCCP module
}

void initUltraSonic(){
    initOutputCompare();
    initInputCapture();
}

/*
u32 readSensor(){
    u32 lowerValue = CCP2BUF;
    u32 higherValue = CCP2BUF;
    u32 diff = higherValue - lowerValue;
    diff = diff >> 1;   //halbe strecke
    // 125 × (24M / 64) / 2 × (64/24MHz) × 343.2m/s) / 1000000 = 2.145 cm (min distance)
    // 25000 × (24M / 64) / 2 × (64/24MHz) × 343.2m/s) / 1000000 = 429 cm (max distance)
    // (64/24M) × 343.2) = 0.0009152
    u32 distance = diff * 9152 / 100000;
    return distance;
}*/


u32 readSensorASM(){
    u32 value = 0;
    u32 multiplier = 9152;
    asm volatile(
        "la $t2, %1         \n\t"       // t2 = &CCP2BUF
        "lh $t0, 0($t2)      \n\t"      //lowerValue
        "lh $t1, 0($t2)      \n\t"      //higherValue
        "sub $t0, $t1, $t0  \n\t"       //diff = higher - lower
        "srl $t0, $t0, 1    \n\t"       //travelTime = diff / 2
        "mul $t0, $t0, %2   \n\t"       //travelTime * 9152
        "li $t1, 100000     \n\t"
        "div $t0, $t1       \n\t"       //distance / 100000
        "mflo %0            \n\t"
        : "=r" (value)
        : "m"(CCP2BUF), "r"(multiplier)
        : "t0", "t1", "t2"
    );
    return value;
}

void __ISR(_CCP2_VECTOR, IPL3SOFT) inputCaptureHandler(void)
{
    distance = readSensorASM();
    if(distance < 420){
        distanceBuffer[bufferIndex] = distance;
        bufferIndex++;
        bufferIndex = bufferIndex % BUFFER_LENGTH;
    }
    IFS2bits.CCP2IF = 0;
}