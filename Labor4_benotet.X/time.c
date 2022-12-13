#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

typedef unsigned char u8;
typedef unsigned int u32;


 char const compile_date[12]   = __DATE__;     // Mmm dd yyyy
 char const compile_time[9]    = __TIME__;     // hh:mm:ss

 void initTime(){
    OSCCON<1>=1;            // enable SOSC
    OSCCON<22>=1;           // SOSC ready
    
    SYSKEY = 0xAA996655; // write Key1 to SYSKEY
    SYSKEY = 0x556699AA; // write Key2 to SYSKEY
    RTCCON1bits.WRLOCK = 0; // clear the lock bit
    SYSKEY = 0; // relock SYSKEY
    
    RTCCON2bits.CLKSEL = 0b00;  // select SOSC
    RTCCON1bits.ON = 0;         // turn off the RTCC
    
    RTCTIME = 0;
    
    RTCTIMEbits.HRTEN = compile_time[0] - 48;
    RTCTIMEbits.HRONE = compile_time[1] - 48;
    RTCTIMEbits.MINTEN = compile_time[3] - 48;
    RTCTIMEbits.MINONE = compile_time[4] - 48;
    RTCTIMEbits.SECTEN = compile_time[6] - 48;
    RTCTIMEbits.SECONE = compile_time[7] - 48;
    
    RTCCON1bits.ON = 1;
    RTCCON1bits.WRLOCK = 1;            // disable the RTCC write

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