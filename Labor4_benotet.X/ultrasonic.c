#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <sys/attribs.h>

typedef unsigned char u8;
typedef unsigned int u32;

void writeLCD (char* str, u32 len);

void initUltraSonic(){
    TRISBbits.TRISB2 = 1;   //RB2 as input
    TRISAbits.TRISA12 = 0;
    
    /*IFS2bits.CCP2IF = 0;
    IPC19bits.CCP2IP = 3;
    IPC19bits.CCP2IS = 1;
    IEC2bits.CCP2IE = 1;
    */
    IFS2bits.CCP2IF = 0;
     
    // Set MCCP operating mode
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
    CCP1PRbits.PRL = 0xEA60; // Configure timebase period
    CCP1RA = 0x0000; // Set the rising edge compare value
    CCP1RB = 0xF; // Set the falling edge compare value to 10us
    CCP1CON1bits.ON = 1; // Turn on MCCP module
    
    
    
    CCP2CON1bits.CCSEL = 1; // Input capture mode
    CCP2CON1bits.CLKSEL = 0; // Set the clock source (Internal Clock ~ 24MHz)
    CCP2CON1bits.MOD = 0b0000; // Capture every rising/falling edge of the event
    CCP2CON1bits.OPS = 0b0001;       //Interrupt every 2nd input capture event
    CCP2CON1bits.TMRPS = 0b11;     // Set the clock prescaler (1:64)
    CCP2CON2bits.ICS = 0;   // ICx Pin
    RPINR2bits.ICM2R = 0b01000;   //ICM2 = RP8 = RB2
    CCP2CON1bits.ON = 1;        // Enable CCP/input capture
}

/*void __ISR(_CCP2_VECTOR, IPL3SOFT) inputCaptureHandler(void)
{
    writeLCD ("hi", 2);
    IFS2bits.CCP2IF = 0;
}*/