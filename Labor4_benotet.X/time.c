#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

typedef unsigned char u8;
typedef unsigned int u32;

void writeLCD (char* str, u32 len);
void setCursor(u8 row, u8 col);
void delay_us(unsigned int us);

 char const compile_date[12]   = __DATE__;     // Mmm dd yyyy
 char const compile_time[9]    = __TIME__;     // hh:mm:ss

 
void stopTime(){
    RTCCON1bits.ON = 0;         // turn off the RTCC
    SYSKEY = 0xAA996655; // write Key1 to SYSKEY
    SYSKEY = 0x556699AA; // write Key2 to SYSKEY
    RTCCON1bits.WRLOCK = 0; // clear the lock bit
    SYSKEY = 0; // relock SYSKEY
}
 
void startTime(){
    RTCCON1bits.ON = 1;
    RTCCON1bits.WRLOCK = 1;            // disable the RTCC write
}

void initTime(){
    OSCCON<1>=1;            // enable SOSC
    OSCCON<22>=1;           // SOSC ready

    stopTime();

    RTCCON2bits.CLKSEL = 0b00;  // select SOSC
    RTCTIME = 0;

    RTCTIMEbits.HRTEN = compile_time[0] - 48;
    RTCTIMEbits.HRONE = compile_time[1] - 48;
    RTCTIMEbits.MINTEN = compile_time[3] - 48;
    RTCTIMEbits.MINONE = compile_time[4] - 48;
    RTCTIMEbits.SECTEN = compile_time[6] - 48;
    RTCTIMEbits.SECONE = compile_time[7] - 48;

    startTime();
  
    //init ADC
    AD1CON1 = 0;
    ANSELCbits.ANSC8 = 1;       //C8 as analog input
    TRISCbits.TRISC8 = 1;       //C8 as analog input
    AD1CHSbits.CH0SA = 14;      //C8, AN14 as pos input
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
    delay_us(1);
    AD1CON1bits.SAMP = 0;
    while(AD1CON1bits.DONE == 0);
    uint16_t data = ADC1BUF0;
    return data;
}
 
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

void convertTime(u32 time, char outTime[8]){
    
    char HRTEN = (time>>28)&0b0111;
    char HRONE = (time>>24)&0b1111;
    char MINTEN = (time>>20)&0b0111;
    char MINONE = (time>>16)&0b1111;
    char SECTEN = (time>>12)&0b0111;
    char SECONE = (time>>8)&0b1111;
    
    sprintf(outTime, "%d%d:%d%d:%d%d",(time>>28)&0b0111, (time>>24)&0b1111, (time>>20)&0b0111, (time>>16)&0b1111, (time>>12)&0b0111, (time>>8)&0b1111);
}

void showTime(){
    char outTime[8];
    u32 time = readTime();
    convertTime(time, outTime);
    setCursor(1, 0);
    writeLCD(outTime, 8);
}

u8 getConfHours(){
    uint16_t adcVal = readADC();
    u8 hours = adcVal / 178;
    return hours;
}

u8 getConfMinSec(){
    uint16_t adcVal = readADC();
    u8 minSec = adcVal / 69;
    return minSec;
}

void setHours(u8 hours){
    u8 ones = hours % 10;
    u8 tens = (hours - ones) / 10;
    RTCTIMEbits.HRONE = ones;
    RTCTIMEbits.HRTEN = tens;
}

void setMinutes(u8 minutes){
    u8 ones = minutes % 10;
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