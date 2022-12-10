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


void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    initI2C();
}

int main(int argc, char** argv) {
    setup();
    
    //initUltraSonic();
    initFallBackUltraSonic();
    
    //initLCD();
    //clearLCD();
    
    while(1){
        u32 distance = readSensorFallBack();
        char str[10];
        sprintf(str, "%d", distance);
        //writeLCD(str, 10);
    }
    

    
    return (EXIT_SUCCESS);
}

