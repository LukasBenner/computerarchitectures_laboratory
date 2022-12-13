/* 
 * File:   main.c
 * Author: lukas
 *
 * Created on December 6, 2022, 9:01 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#define MENU_LIVE_DATA 0
#define MENU_TIME_CONF 1
#define CONF_HOURS 0
#define CONF_MIN 1
#define CONF_SEC 2
#define SHOW_BAR 0
#define SHOW_TIME 1

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
void stopTime();
void startTime();
u8 getConfHours();
u8 getConfMinSec();
void setSeconds(u8 seconds);
void setMinutes(u8 minutes);
void setHours(u8 hours);

extern u32 distanceBuffer[];
u8 barTimeState = 0;
u8 menuState = MENU_LIVE_DATA;
u8 timeConfState = CONF_HOURS;

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
    TRISBSET = 0b1000000000;    //Button on Port B9 as input
    TRISCSET = 0b10000000000;   //Button on Port C10 as input
}

u8 checkS1Button(u8 buttonState){
    uint32_t button = PORTBbits.RB9;
    if(button == 0){  
        buttonState = 1 - buttonState; //toggle
        while(button == 0)     //prevent multiple clicks in one press
        {
            button = PORTBbits.RB9;
        }
        clearLCD();
    }
    return buttonState;
}

u8 checkS2Button(u8 buttonState){
    uint32_t button = PORTCbits.RC10;
    if(button == 0){  
        buttonState++;
        buttonState = buttonState % 3;
        while(button == 0)     //prevent multiple clicks in one press
        {
            button = PORTCbits.RC10;
        }
        clearLCD();
    }
    return buttonState;
}

u8 checkS3Button(u8 buttonState){

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



void showLiveData(){
    u32 average = distanceBuffer[0] + distanceBuffer[1] + distanceBuffer[2] + distanceBuffer[3];
    average = average >> 2;
    char str[16];
    sprintf(str, "Distance: %03d cm", average);
    setCursor(0, 0);
    writeLCD(str, 16);

    barTimeState = checkS3Button(barTimeState);
    if(barTimeState == SHOW_TIME){
        showTime();
    }
    else if(barTimeState == SHOW_BAR){
        showBar(average);
    }
}

void showTimeMenu(){
    timeConfState = checkS2Button(timeConfState);
    u8 time = 0;
    if(timeConfState == CONF_HOURS){
        setCursor(0,0);
        writeLCD("Set hours:", 10);
        time = getConfHours();
        setHours(time);
    }
    else if(timeConfState == CONF_MIN){
        setCursor(0,0);
        writeLCD("Set minutes:", 12);
        time = getConfMinSec();
        setMinutes(time);
    }
    else if(timeConfState == CONF_SEC){
        setCursor(0,0);
        writeLCD("Set seconds:", 12);
        time = getConfMinSec();
        setSeconds(time);
    }
    setCursor(1,0);
    char timeStr[2];
    sprintf(timeStr, "%2d", time);
    writeLCD(timeStr, 2);
}

void changeState(u8 oldState, u8 newState){
    if(oldState == MENU_LIVE_DATA && newState == MENU_TIME_CONF){
        stopTime();
    }
    else if(oldState == MENU_TIME_CONF && newState == MENU_LIVE_DATA){
        startTime();
    }
}

int main(int argc, char** argv) {
    setup();
    while(1){
        u8 newState = checkS1Button(menuState);
        changeState(menuState, newState);
        menuState = newState;
        
        if(menuState == MENU_LIVE_DATA){
            showLiveData();
        }
        if(menuState == MENU_TIME_CONF){
            showTimeMenu();
        }
    }
    return (EXIT_SUCCESS);
}

