#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

typedef unsigned char u8;
typedef unsigned int u32;

/* BEGIN FORWARD DECLARATIONS */
void writeLCD (char* str, u32 len);
void setCursor(u8 row, u8 col);
void delay_us(unsigned int us);
/* END FORWARD DECLARATIONS */

 char const compile_date[12]   = __DATE__;     // Mmm dd yyyy
 char const compile_time[9]    = __TIME__;     // hh:mm:ss

 
/* stop the RTCC module and unlock the RTCC registers */
void stopTime(){
    /* This is from PIC32_RTCC.pdf page 5 */
    RTCCON1bits.ON = 0;         // turn off the RTCC
    SYSKEY = 0xAA996655;        // write Key1 to SYSKEY
    SYSKEY = 0x556699AA;        // write Key2 to SYSKEY
    RTCCON1bits.WRLOCK = 0;     // disable the RTCC write
    SYSKEY = 0;                 // relock SYSKEY
}
 
/* start the RTCC module and lock the RTCC registers */
void startTime(){
    RTCCON1bits.ON = 1;
    RTCCON1bits.WRLOCK = 1;     // disable the RTCC write
}

/* mehod to init the ADC to be used with the potentiometer*/
void initADC(){
    AD1CON1 = 0;
    ANSELCbits.ANSC8 = 1;       // C8 as analog input
    TRISCbits.TRISC8 = 1;       // C8 as analog input
    AD1CHSbits.CH0SA = 14;      // C8, AN14 as pos input
    AD1CON1bits.MODE12 = 1;
    AD1CON1bits.FORM = 0b000;   // uINT 16

    AD1CON3bits.ADRC = 1;       // FRC as clock
                                // TAD min = 300ns
                                // Set TAD to 4 * TSRC = 4 * 125 ns = 500ns
    AD1CON3bits.ADCS = 2;
    AD1CON1bits.ON = 1;
}

/* method to init RTCC and compile time */
void initTime(){
    initADC();      // adc is used to read the potentiometer
    stopTime();     // stop RTCC and unlock RTCC registers

    OSCCONbits.SOSCEN = 1;      // enable SOSC
    RTCCON2bits.CLKSEL = 0b00;  // select SOSC
    RTCTIME = 0;                // clear time register

    RTCTIMEbits.HRTEN = compile_time[0] - 48;   // -48 to get correct number -> 48 = 0 in ASCII
    RTCTIMEbits.HRONE = compile_time[1] - 48;
    RTCTIMEbits.MINTEN = compile_time[3] - 48;
    RTCTIMEbits.MINONE = compile_time[4] - 48;
    RTCTIMEbits.SECTEN = compile_time[6] - 48;
    RTCTIMEbits.SECONE = compile_time[7] - 48;

    startTime();    // start RTCC and lock RTCC registers
}

/* read value of potentiometer from ADC */
uint16_t readPotentiometer(){
    AD1CON1bits.SAMP = 1;           // start sampling
    delay_us(1);
    AD1CON1bits.SAMP = 0;           // end sampling
    while(AD1CON1bits.DONE == 0);   // wait for conversion to finish
    uint16_t data = ADC1BUF0;
    return data;
}
 
/* method to read current time from RTCC */
u32 readTime()
{
    u32 timeCopy1, timeCopy2;
    if (RTCSTATbits.SYNC == 0)
    {
        // Time registers may be read safely
        return (RTCTIME); // return time
    }
    else
    {
        // Time registers may change during software read
        // read time twice and compare result, retry until a match occurs
        while ( (timeCopy1 = RTCTIME) != (timeCopy2 = RTCTIME) );
        return (timeCopy1); // return time when both reads matched
    }
}

/* method to convert time to string output */
void convertTimeToString(u32 time, char outTime[8]){
    /* The variable time is holds the value of RTCTIME.
     * Create a string and emplace the placeholders with the 
     * shifted and masked bits of RTCTIME.
    */
    sprintf(outTime, "%d%d:%d%d:%d%d", 
            (time>>28)&0b0111,
            (time>>24)&0b1111,
            (time>>20)&0b0111,
            (time>>16)&0b1111,
            (time>>12)&0b0111,
            (time>>8)&0b1111);
}

/* method to display the RTCC time on LCD display */
void showTime(){
    char outTime[8];
    u32 time = readTime();                  // get current time
    convertTimeToString(time, outTime);     // convert time to string
    setCursor(1, 0);
    writeLCD(outTime, 8);                   // display time on display
}

/* method to get value between 0 and 23 from potentiometer */
u8 getConfHours(){
    uint16_t adcVal = readPotentiometer();  // possible values of adcVal are [0, 4096]
    u8 time = adcVal / 178;                // 23 hours = 4096 / 178
    return time;
}

/* method to get value between 0 and 59 from potentiometer */
u8 getConfMinSec(){
    uint16_t adcVal = readPotentiometer();  // possible values of adcVal are [0, 4096]
    u8 time = adcVal / 69;                // 59 min or sec = 4096 / 69
    return time;
}

void setHours(u8 hours){
    u8 ones = hours % 10;               // split value into ones ant tens
    u8 tens = (hours - ones) / 10;
    RTCTIMEbits.HRONE = ones;           // save digits of time to registers
    RTCTIMEbits.HRTEN = tens;
}

void setMinutes(u8 minutes){
    u8 ones = minutes % 10;             // same as above
    u8 tens = (minutes - ones) / 10;
    RTCTIMEbits.MINONE = ones;
    RTCTIMEbits.MINTEN = tens;
}

void setSeconds(u8 seconds){
    u8 ones = seconds % 10;
    u8 tens = (seconds - ones) / 10;
    RTCTIMEbits.SECONE = ones;
    RTCTIMEbits.SECTEN = tens;
}