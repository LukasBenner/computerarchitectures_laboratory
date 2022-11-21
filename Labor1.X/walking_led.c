
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/attribs.h>

void walkingLed(){
    
    /*
     * LEDs an Port A10 - A14
     * LED mit 120 Ohm Vorwiederstand
     * 3,3V - 2,1V = 1,2V
     * R = U/I = 1,2V / 10mA = 120 Ohm
     * 
     * 1s / 5 = 0.2s
     * prescaler auf 1 -> Timer auf 0x2000 -> 0x2000 × (1/32768Hz) = 250 ms
     * T1CON bit 1 auf 1 -> sosc
     * T1CON bit 15 auf 1 -> Timer ist an
     * T1CON bit 7 auf 0 -> MUX nimmt Eingang 0
     * OSCCON bit 1 auf 1 -> SOSC ist an
     * */
    TRISACLR = 0b0111110000000000;  //LEDs as output
    TRISACLR = 0x7C00;              // LEDs are off
    
    T1CON = 0x0;                // Stops the Timer1 and resets the control register
    TMR1 = 0x0;                 // Clear timer register
    IFS0bits.T1IF = 0;          // Clear int status reg
    OSCCONbits.SOSCEN = 1;      // Enable SOSC Source        
    T1CONbits.TCS = 1;          // Timer source to sosc
    T1CONbits.TCKPS = 0b00;     // Prescaler to 1
    PR1 = 0x2000;               // Timer to 0x2000
    T1CONbits.ON = 1;           // Start Timer

    LATASET = 0x0400;           // turn on led 10
    
    uint32_t baseAddress = 0x0400;  // address of led10
    uint8_t i = 0;
    int8_t direction = 1;
    
    while(1){
        if(IFS0bits.T1IF == 1){
            // timer flag is set
            /*
             * Shift baseAddress by i and turn LED off
             * Increment i by direction
             * If i is 4 or 0, change direction
             */
            //turn off current LED
            LATACLR = baseAddress << i;
            //increment i
            i += direction;
            //check modulo
            if(i % 4 == 0)
            {
                //turn direction
                direction = -direction;
            }
            
            //activate next LED
            LATASET = baseAddress << i;
            IFS0bits.T1IF = 0;      // unset timer interrupt flag
        }
    }
}