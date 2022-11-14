
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/attribs.h>

void externLedBlink(){
    
    /*
     * LED an Port A14
     * Button S1 an Port B9
     * LED mit 120 Ohm Vorwiederstand
     * 3,3V - 2,1V = 1,2V
     * R = U/I = 1,2V / 10mA = 120 Ohm
     * 
     * prescaler auf 64 -> Timer auf 2^9 -> 2^9 * 64 = 32768
     * 
     * T1CON bit 1 auf 1 -> sosc
     * T1CON bit 15 auf 1 -> Timer ist an
     * T1CON bit 7 auf 0 -> MUX nimmt Eingang 0
     * OSCCON bit 1 auf 1 -> SOSC ist an
     * 
     * */
    TRISBSET = 0b01000000000;   //Button on Port B9 as input
    TRISACLR = 0x4000;          //LED on Port A14 as output
    
    T1CON = 0x0;                // Stops the Timer1 and resets the control register
    TMR1 = 0x0;                 // Clear timer register
    IFS0bits.T1IF = 0;              // Clear Interrupt status register
    OSCCONbits.SOSCEN = 1;          // Enable SOSC Source
    T1CON = T1CON = 0x2;        // Timer source to sosc
    T1CONbits.TCKPS = 0b10;         // Prescaler to 64
    PR1SET = 0x200;             // Timer to 2^9 = 512
    T1CONSET = 0x8000;          // Start Timer
    
    LATACLR = 0x4000;          // LED is off
    bool ledState = false;
    
    while(1){
        uint32_t portB_value = PORTB;
        uint8_t button = (portB_value & 0b0100000000) >> 9;
        
        if(button == 1){
            
            ledState = !ledState;  //Toggle LED State
            
            while(button == 1){     //prevent multiple clicks in one press
                portB_value = PORTB;
                button = (portB_value & 0b0100000000) >> 9;
            }
        }
        
        if(ledState && IFS0bits.T1IF == 1){
            // ledState is true and timer flag is set
            LATAINV = 0x4000;  // Toggle LED on Port A14
            IFS0bits.T1IF = 0;      // unset timer interrupt flag
        }
        
    }
}