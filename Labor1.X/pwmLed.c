
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/attribs.h>


void delay_us(unsigned int us);

void pwmLed(){
    
    /*
     * LEDs an Port A10
     * LED mit 120 Ohm Vorwiederstand
     * 3,3V - 2,1V = 1,2V
     * R = U/I = 1,2V / 10mA = 120 Ohm
     * 
     * */
    
    
    TRISACLR = 0b010000000000;      // LED as output
    TRISACLR = 0b010000000000;      // LED is off
    
    // Set MCCP operating mode
    CCP1CON1bits.CCSEL = 0; // Set MCCP operating mode (OC mode)
    CCP1CON1bits.T32 = 0; // Set timebase width (16-bit)
    CCP1CON1bits.MOD = 0b0101; // Set mode (Buffered Dual-Compare/PWM mode)
    
    
    CCP1CON1bits.TMRSYNC = 0; // Set timebase synchronization (not Synchronized)
    CCP1CON1bits.CLKSEL = 0b000; // Set the clock source (Internal Clock ~ 24MHz)
    CCP1CON1bits.TMRPS = 0b00; // Set the clock prescaler (1:1)
    CCP1CON1bits.TRIGEN = 0; // Set Sync/Triggered mode (Synchronous)
    CCP1CON1bits.SYNC = 0b00000; // Select Sync/Trigger source (Self-sync)
    //Configure MCCP output for PWM signal
    CCP1CON2bits.OCAEN = 1; // Enable desired output signals (OC1A)
    CCP1CON3bits.OUTM = 0b000; // Set advanced output modes (Standard output)
    CCP1CON3bits.POLACE = 0; // Configure output polarity (Active High)
    CCP1TMRbits.TMRL = 0x0000; // Initialize timer prior to enable module.
    CCP1PRbits.PRL = 0xFFFF; // Configure timebase period
    CCP1RA = 0x0000; // Set the rising edge compare value
    CCP1RB = 0x0000; // Set the falling edge compare value
    CCP1CON1bits.ON = 1; // Turn on MCCP module

    
    int32_t direction = 1;
    uint32_t value = 0;
    
    while(1){
        value += direction;
        CCP1RB = value;
        delay_us(10);
        
        if(value == 0xFFFF || value == 0){
            direction = -direction;
        }
    }
}
