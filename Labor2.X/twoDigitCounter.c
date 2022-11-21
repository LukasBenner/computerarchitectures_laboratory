#include <stdint.h>
#include <xc.h>

typedef unsigned int u32;

void sevenSegWrite();
void delay_us();

void writeTensDigit(u32 digit){
    LATBbits.LATB2 = 0;
    LATBbits.LATB1 = 1;
    // maybe a delay is needed here
    sevenSegWrite(digit);
}

void writeOnesDigit(u32 digit){
    LATBbits.LATB1 = 0;
    LATBbits.LATB2 = 1;
    // maybe a delay is needed here
    sevenSegWrite(digit);
}

void twoDigitCounter(){
    
    T1CON = 0x0;                // Stops the Timer1 and resets the control register
    TMR1 = 0x0;                 // Clear timer register
    IFS0bits.T1IF = 0;          // Clear int status reg
    OSCCONbits.SOSCEN = 1;      // Enable SOSC Source        
    T1CONbits.TCS = 1;          // Timer source to sosc
    T1CONbits.TCKPS = 0b10;     // Prescaler to 64
    PR1 = 0x10;                // Timer to 2^8 = 256
    // Timer is set to 32Hz
    T1CONbits.ON = 1;           // Start Timer
    
    u32 ones = 0;
    u32 tens = 0;
    while(1){
        
        if(IFS0bits.T1IF == 1){
            ones ++;
            if(ones == 10){
                tens ++;
                ones = 0;
            }
            if(tens == 10){
                ones = 0;
                tens = 0;
            }
            IFS0bits.T1IF = 0; //Reset Timer
        }
        
        writeTensDigit(tens);
        writeOnesDigit(ones);
        
    }
}