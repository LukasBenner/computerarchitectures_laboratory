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


void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    initI2C();
}

int main(int argc, char** argv) {
    setup();
    
    initLCD();
    clearLCD();
    while(1){
        setCursor(0,0);
        writeLCD("Hello World", 11);
        delay_us(1000000);
    }
   
    
    return (EXIT_SUCCESS);
}

