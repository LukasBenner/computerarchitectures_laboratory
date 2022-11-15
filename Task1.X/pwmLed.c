
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/attribs.h>

void pwmLed(){
    
    /*
     * LEDs an Port A10
     * LED mit 120 Ohm Vorwiederstand
     * 3,3V - 2,1V = 1,2V
     * R = U/I = 1,2V / 10mA = 120 Ohm
     * 
     * 1s / 5 = 0.2s
     * prescaler auf 1 -> Timer auf 0x199D -> 0x199D × (1/32768Hz) = 200,1 ms
     * T1CON bit 1 auf 1 -> sosc
     * T1CON bit 15 auf 1 -> Timer ist an
     * T1CON bit 7 auf 0 -> MUX nimmt Eingang 0
     * OSCCON bit 1 auf 1 -> SOSC ist an
     * */
    
    
    TRISACLR = 0b010000000000;      // LED as output
    TRISACLR = 0b010000000000;      // LED is off
    
    // Set MCCP operating mode
    CCP1CON1bits.CCSEL = 0; // Set MCCP operating mode (OC mode)
    CCP1CON1bits.MOD = 0b0101; // Set mode (Buffered Dual-Compare/PWM mode)
    //Configure MCCP Timebase
    CCP1CON1bits.T32 = 0; // Set timebase width (16-bit)
    CCP1CON1bits.TMRSYNC = 0; // Set timebase synchronization (Synchronized)
    CCP1CON1bits.CLKSEL = 0b000; // Set the clock source (Tcy)
    CCP1CON1bits.TMRPS = 0b00; // Set the clock prescaler (1:1)
    CCP1CON1bits.TRIGEN = 0; // Set Sync/Triggered mode (Synchronous)
    CCP1CON1bits.SYNC = 0b00000; // Select Sync/Trigger source (Self-sync)
    //Configure MCCP output for PWM signal
    CCP1CON2bits.OCAEN = 1; // Enable desired output signals (OC1A)
    CCP1CON3bits.OUTM = 0b000; // Set advanced output modes (Standard output)
    CCP1CON3bits.POLACE = 0; // Configure output polarity (Active High)
    CCP1TMRbits.TMRL = 0x0000; // Initialize timer prior to enable module.
    CCP1PRbits.PRL = 0xFFFF; // Configure timebase period
    CCP1RA = 0x1000; // Set the rising edge compare value
    CCP1RB = 0x8000; // Set the falling edge compare value
    CCP1CON1bits.ON = 1; // Turn on MCCP module


}
