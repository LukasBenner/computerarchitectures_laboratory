/* 
 * File:   main.c
 * Author: lukas
 *
 * Created on December 6, 2022, 9:01 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

typedef unsigned char u8;
typedef unsigned int u32;

void SYSTEM_Initialize(void);
void initI2C();
void startI2C();
void initLCD();
u8 sendI2C(u8 instr, u8 value);
void initLCD();
void clearLCD();
void writeLCD (char* str, u32 len);
void setCursor(u8 row, u8 col);
void delay_us(unsigned int us);
void initUltraSonic();
u32 readSensorFallBack();
void initFallBackUltraSonic();

#define BUFFER_LENGTH 2

u32 distance = 0;
u8 bufferIndex = 0;
u32 distanceBuffer[BUFFER_LENGTH];

 u8 const compile_date[12]   = __DATE__;     // Mmm dd yyyy
 u8 const compile_time[9]    = __TIME__;     // hh:mm:ss

 void initTime(){
    OSCCON<1>=1;            // enable SOSC
    OSCCON<22>=1;           // SOSC ready
    RTCWREN = 1;            // To write RTCTIME and RTCDATE

    RTCCONCLR=0x8000;       // turn off the RTCC
    while(RTCCON&0x40);     // wait for clock to be turned off
    RTCTIME = compile_time; // update the time
    RTCDATE = compile_date; // update the date
    RTCCONSET = 0x8000;     // turn on the RTCC
    while(!(RTCCON&0x40));  // wait for clock to be turned on
    RTCWREN = 0;            // disable the RTCC write

 }
 
 
void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    initI2C();

    initTime();
    
}

int main(int argc, char** argv) {
    setup();
    
    //initUltraSonic();
    initFallBackUltraSonic();
    
    //initLCD();
    //clearLCD();
    
    while(1){
        distance = readSensorFallBack();
        //max value of distance should be < 450
        if(distance < 450){
            distanceBuffer[bufferIndex] = distance;
            bufferIndex++;
        }
        
        if(bufferIndex == BUFFER_LENGTH){
            u32 average = (distanceBuffer[0] + distanceBuffer[1]);
            average = average >> 1;
            char str[16];
            sprintf(str, "Distance: %d cm", average);
            //setCursor(0, 0);
            //writeLCD(str, 16);
            bufferIndex = 0;
        }
    }
    return (EXIT_SUCCESS);
}

