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
void showBar(u32 distance);
void showTime();

extern u32 distanceBuffer[];

void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    initI2C();
    initTime();
    
    initUltraSonic();

    initLCD();
    clearLCD();
    
    //init s3 to switch mode
    TRISCSET = 0b010000;        //Button on Port C4 as input
}

u8 checkButton(u8 buttonState){

    uint32_t button = PORTCbits.RC4;
    if(button == 0){  
        buttonState = 1 - buttonState; //toggle
        while(button == 0)     //prevent multiple clicks in one press
        {
            button = PORTCbits.RC4;
        }
        clearLCD();
    }
    return buttonState;
}

int main(int argc, char** argv) {
    setup();
    u8 buttonState = 0;
    
    while(1){
        u32 average = distanceBuffer[0] + distanceBuffer[1] + distanceBuffer[2] + distanceBuffer[3];
        average = average >> 2;
        char str[16];
        sprintf(str, "Distance: %03d cm", average);
        setCursor(0, 0);
        writeLCD(str, 16);
        
        
        buttonState = checkButton(buttonState);
        if(buttonState == 0){
            showTime();
        }
        else{
            showBar(average);
        }
    }
    return (EXIT_SUCCESS);
}

