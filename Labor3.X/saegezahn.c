#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>

void delay_us();

void initVoltageRefUnit(){
    DAC1CONbits.DACOE = 1;      // Voltage level is output on the CVREF pin
    DAC1CONbits.REFSEL = 0b11;  // Reference voltage is AV DD
    DAC1CONbits.ON = 1;         // Voltage reference is enabled
    
}

void saegezahnDelay(){
    initVoltageRefUnit();
    while(1){
        for (int i = 0; i<32; i++){
            DAC1CONbits.DACDAT = i;
            delay_us(62);               // (1/500Hz) / 32
        }
    }
}

void saegezahnInterrupt(){
    
    initVoltageRefUnit();
   
    T1CON = 0x0;                // Stops the Timer1 and resets the control register
    TMR1 = 0x0;                 // Clear timer register
    IFS0bits.T1IF = 0;          // Clear int status reg
    
    IPC4bits.T1IP = 3;          // Priority level 3
    IPC4bits.T1IS = 1;          // Subpriority level 1
    IEC0bits.T1IE = 1;          // Enable timer interrupts
    
    T1CONbits.TCS = 0;          // Internal PB Clock   24 MHz / 16kHz = 1500
    T1CONbits.TCKPS = 0b00;     // prescaler to 1
    PR1 = 1500;                 // Timer to 1500
    // Timer is set to 1Hz
    T1CONbits.ON = 1;           // Start Timer
}

void initADC(){
    AD1CON1 = 0;
    ANSELCbits.ANSC8 = 1;       //C8 as analog input
    TRISCbits.TRISC8 = 1;       //C8 as analog input
    AD1CHSbits.CH0SA = 0b01110;  //C0, AN12 as pos input
    AD1CON1bits.MODE12 = 1;
    AD1CON1bits.FORM = 0b000;   // uINT 16
    
    AD1CON3bits.ADRC = 1;       // FRC as clock
    // TAD min = 300ns
    // Set TAD to 4 * TSRC = 4 * 125 ns = 500ns
    AD1CON3bits.ADCS = 2;
    AD1CON1bits.ON = 1;
}

uint16_t readADC(){
    AD1CON1bits.SAMP = 1;
    delay_us(10);
    AD1CON1bits.SAMP = 0;
    while(AD1CON1bits.DONE == 0);
    //AD1CON1bits.DONE = 0;
    uint16_t data = ADC1BUF0;
    return data;
}

void saegezahnVariableInterrupt(){
    
    initVoltageRefUnit();
    initADC();
   
    T1CON = 0x0;                // Stops the Timer1 and resets the control register
    TMR1 = 0x0;                 // Clear timer register
    IFS0bits.T1IF = 0;          // Clear int status reg
    
    IPC4bits.T1IP = 3;          // Priority level 3
    IPC4bits.T1IS = 1;          // Subpriority level 1
    IEC0bits.T1IE = 1;          // Enable timer interrupts
    
    T1CONbits.TCS = 0;          // Internal PB Clock   24 MHz / 16kHz = 1500
    T1CONbits.TCKPS = 0b00;     // prescaler to 1
    PR1 = 1500;                 // Timer to 1500
    // Timer is set to 1Hz
    T1CONbits.ON = 1;           // Start Timer
    
    while(1){
        uint16_t adcVal = readADC();
        // offset 3750   steigung von 3000 / 4096 = 0.732
        uint16_t data = 3750 - (adcVal * 732) / 1000;
        PR1 = data;
    }
    
}

void nextOutput(){
    uint8_t i = DAC1CONbits.DACDAT;     // load
    i++;                                // increment
    i = i % 32;                         // mod 32
    DAC1CONbits.DACDAT = i;             // write back
}

/*void __ISR(_TIMER_1_VECTOR, IPL3SOFT) saegeZahnHandler(void)
{
    nextOutput();
    IFS0bits.T1IF = 0;
}*/