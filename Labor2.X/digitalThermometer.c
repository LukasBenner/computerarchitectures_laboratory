#include <stdint.h>
#include <xc.h>

typedef unsigned int u32;

void sevenSegWrite();
void delay_us();
void writeOnesDigit(u32 digit);
void writeTensDigit(u32 digit);

void initADC(){
    AD1CON1 = 0;
    //ANSELCbits.ANSC0 = 1;
    ANSELAbits.ANSA6 = 1;
    TRISAbits.TRISA6 = 1;
    TRISCbits.TRISC0 = 1;       //C0 as analog input
    AD1CHSbits.CH0NA = 0b000;   //VREFL as neg input
    //AD1CHSbits.CH0SA = 0b01100;  //C0, AN12 as pos input
    AD1CHSbits.CH0SA = 0b010011;
    AD1CON1bits.MODE12 = 1;
    AD1CON1bits.FORM = 0b000;   // uINT 16
    
    AD1CON3bits.ADRC = 1;       // FRC as clock
    // TAD min = 300ns
    // Set TAD to 4 * TSRC = 4 * 125 ns = 500ns
    AD1CON3bits.ADCS = 2;
    AD1CON1bits.ON = 1;
}


u32 readADC(){
    AD1CON1bits.SAMP = 1;
    delay_us(10);
    AD1CON1bits.SAMP = 0;
    while(AD1CON1bits.DONE == 0);
    //AD1CON1bits.DONE = 0;
    u32 data = ADC1BUF0;
    return data;
}

u32 scaleTemperature(u32 data){
    // 0.5V / (3.3V / (2^12)) = 620.6 -> 0°C
    // (0.01V / 1°C) / (3.3V / (2^12)) = 12.4 / 1°C
    // temp = (81 * x - 50080) / 1000
    u32 temp = (81 * data - 50080) / 1000;
    return temp;
}

void digitalThermometer(){
    initADC();
    
    T1CON = 0x0;                // Stops the Timer1 and resets the control register
    TMR1 = 0x0;                 // Clear timer register
    IFS0bits.T1IF = 0;          // Clear int status reg
    OSCCONbits.SOSCEN = 1;      // Enable SOSC Source        
    T1CONbits.TCS = 1;          // Timer source to sosc
    T1CONbits.TCKPS = 0b10;     // Prescaler to 64
    PR1 = 0x100;                // Timer to 2^8 = 256
    // Timer is set to 1Hz
    T1CONbits.ON = 1;           // Start Timer
    
    u32 ones = 0;
    u32 tens = 0;
    
    while(1){
        
        if(IFS0bits.T1IF == 1){
            u32 data = readADC();
            u32 temp = scaleTemperature(data);
        
            ones = temp % 10;
            tens = (temp - ones) / 10;
            IFS0bits.T1IF = 0; //Reset Timer
        }
        
        writeTensDigit(tens);
        writeOnesDigit(ones);
    }
    
}