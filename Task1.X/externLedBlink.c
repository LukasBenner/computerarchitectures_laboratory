
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
     * prescaler auf 64 -> Timer auf 2^8 -> 2^9 * 64 = 16?384
     * 
     * T1CON bit 1 auf 1 -> sosc
     * T1CON bit 15 auf 1 -> Timer ist an
     * T1CON bit 7 auf 0 -> MUX nimmt Eingang 0
     * OSCCON bit 1 auf 1 -> SOSC ist an
     * 
     * */
    TRISBSET = 0b001000000000;    //Button on Port B9 as input
    TRISACLR = 0x4000;           //LED on Port A14 as output
    LATACLR = 0x4000;
    
    T1CON = 0x0;                // Stops the Timer1 and resets the control register
    TMR1 = 0x0;                 // Clear timer register
    IFS0bits.T1IF = 0;          // Clear int status reg
    OSCCONbits.SOSCEN = 1;          // Enable SOSC Source        
    T1CONbits.TCS = 1;              // Timer source to sosc
    T1CONbits.TCKPS = 0b10;         // Prescaler to 64
    PR1 = 0x100;             // Timer to 2^8 = 256
    T1CONbits.ON = 1;           // Start Timer
    
    uint8_t ledState = 0;
    
    while(1){
        uint32_t button = PORTBbits.RB9;
        
        if(button == 0){
            
            if(ledState == 1){
                ledState = 0;
                LATACLR = 0x4000;
            }
             
            else
                ledState = 1;
            
            while(button == 0){     //prevent multiple clicks in one press
                button = PORTBbits.RB9;
            }
        }
        
        uint32_t timerStatus = IFS0bits.T1IF;
        if(ledState == 1 && timerStatus == 1){
            // ledState is true and timer flag is set
            LATAINV = 0x4000;  //Toggle LED on Port A14
            IFS0bits.T1IF = 0;
        }
        
    }
}