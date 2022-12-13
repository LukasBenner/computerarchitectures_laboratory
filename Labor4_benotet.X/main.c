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
void initLCD();
void initTime();
void clearLCD();
void writeLCD (char* str, u32 len);
void setCursor(u8 row, u8 col);
void delay_us(unsigned int us);
void initUltraSonic();
u32 readSensorFallBack();
void initFallBackUltraSonic();
u32 readTime();
void convertTime(u32 time, char outTime[8]);

extern u32 distanceBuffer[];

extern char const compile_time[9];
  
void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    initI2C();
    initTime();
    
    initUltraSonic();

    initLCD();
    clearLCD();
}

int main(int argc, char** argv) {
    setup();
    
    while(1){
        u32 average = distanceBuffer[0] + distanceBuffer[1] + distanceBuffer[2] + distanceBuffer[3];
        average = average >> 2;
        char str[16];
        sprintf(str, "Distance: %03d cm", average);
        setCursor(0, 0);
        writeLCD(str, 16);
        
        char outTime[8];
        u32 time = readTime();
        convertTime(time, outTime);
        setCursor(1, 0);
        writeLCD(outTime, 8);
    }
    return (EXIT_SUCCESS);
}

