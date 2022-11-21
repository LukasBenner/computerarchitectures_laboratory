#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>

typedef unsigned int u32;

void sevenSegWrite();
void delay_us();
void initADC();
u32 readADC();
u32 scaleTemperature(u32 data);
void writeOnesDigit(u32 digit);
void writeTensDigit(u32 digit);

u32 ones = 0;
u32 tens = 0;

void digitalInterruptThermometer(){
    initADC();
    
    T1CON = 0x0;                // Stops the Timer1 and resets the control register
    TMR1 = 0x0;                 // Clear timer register
    IFS0bits.T1IF = 0;          // Clear int status reg
    
    IPC4bits.T1IP = 3;          // Priority level 3
    IPC4bits.T1IS = 1;          // Subpriority level 1
    IEC0bits.T1IE = 1;          // Enable timer interrupts
    
    OSCCONbits.SOSCEN = 1;      // Enable SOSC Source        
    T1CONbits.TCS = 1;          // Timer source to sosc
    T1CONbits.TCKPS = 0b10;     // Prescaler to 64
    PR1 = 0x200;                // Timer to 2^8 = 256
    // Timer is set to 1Hz
    T1CONbits.ON = 1;           // Start Timer
    
    while(1){   
        writeTensDigit(tens);
        writeOnesDigit(ones);
    }
}


void __ISR(_TIMER_1_VECTOR, IPL3SOFT) Timer1Handler(void)
{
    u32 data = readADC();
    u32 temp = scaleTemperature(data);

    ones = temp % 10;
    tens = (temp - ones) / 10;
    IFS0bits.T1IF = 0;
}