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

/* BEGIN FORWARD DECLARATIONS */
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
/* END FORWARD DECLARATIONS */

extern u32 distanceBuffer[];            // Ring Buffer with 4 distance values
u8 barTimeState = SHOW_BAR;             // set barTimeState to show the bar
u8 menuState = MENU_LIVE_DATA;          // set menuState to live data
u8 timeConfState = CONF_HOURS;          // set confState to hours

/* method to setup all configurations of the hardware */
void setup() { 
    SYSTEM_Initialize();                // set 24 MHz clock for CPU and Peripheral Bus
                                        // clock period = 41,667 ns = 0,0417 us
    initI2C();                          // init I2C configurations
    initTime();                         // init RTCC module and set tine to compile time
    
    initUltraSonic();                   // init ultrasonic configurations

    initLCD();                          // init LCD display
    clearLCD();                         // clear LSD display
    
    TRISCSET = 0b010000;                // Button on Port C4 as input
    TRISBSET = 0b1000000000;            // Button on Port B9 as input
    TRISCSET = 0b10000000000;           // Button on Port C10 as input
}

/* method to check state of button S1 (set time) */
u8 checkS1Button(u8 buttonState){
    uint32_t button = PORTBbits.RB9;    // Button S1 on Port B9
    if(button == 0){  
        buttonState = 1 - buttonState;  // toggle button state
        while(button == 0)              // prevent multiple clicks in one press
        {
            button = PORTBbits.RB9;
        }
        clearLCD();                     // clear display to prevent overlapping text
    }
    return buttonState;
}

/* method to check state of button S2 (set hours, minutes or seconds) */
u8 checkS2Button(u8 buttonState){
    uint32_t button = PORTCbits.RC10;   // Button S2 on Port C10
    if(button == 0){  
        buttonState++;
        buttonState = buttonState % 3;  // 3 button state -> hours, minutes, seconds
        while(button == 0)              // prevent multiple clicks in one press
        {
            button = PORTCbits.RC10;
        }
        clearLCD();                     // clear display to prevent overlapping text
    }
    return buttonState;
}


/* method to check state of button S3 (distance bar or time)*/
u8 checkS3Button(u8 buttonState){

    uint32_t button = PORTCbits.RC4;    // Button S3 on Port C4
    if(button == 0){  
        buttonState = 1 - buttonState;  // toggle button state
        while(button == 0)              // prevent multiple clicks in one press
        {
            button = PORTCbits.RC4;
        }
        clearLCD();                     // clear display to prevent overlapping of time and bar
    }
    return buttonState;
}


/* method to show live data (distance, time, bar)*/
void showLiveData(){
    u32 average = distanceBuffer[0] + distanceBuffer[1] 
        + distanceBuffer[2] + distanceBuffer[3];    // add last 4 distance values from buffer
    average = average >> 2;                         // build average -> average / 4
    char str[16];
    sprintf(str, "Distance: %03d cm", average);     // create distance message
    setCursor(0, 0);                                
    writeLCD(str, 16);                              // write distance message

    // check if time or bar should be displayed
    barTimeState = checkS3Button(barTimeState);
    if(barTimeState == SHOW_TIME){
        showTime();
    }
    else if(barTimeState == SHOW_BAR){
        showBar(average);
    }
}

/* method to set own time */
void showTimeMenu(){
    // check state of button -> set hours, minutes or seconds
    timeConfState = checkS2Button(timeConfState);
    u8 time = 0;
    if(timeConfState == CONF_HOURS){                // configure hours
        setCursor(0,0);
        writeLCD("Set hours:", 10);
        time = getConfHours();                      // get configured hours
        setHours(time);                             // set configured hours
    }
    else if(timeConfState == CONF_MIN){             // configure minutes
        setCursor(0,0);
        writeLCD("Set minutes:", 12);
        time = getConfMinSec();                     // get configured minutes
        setMinutes(time);                           // set configured minutes
    }
    else if(timeConfState == CONF_SEC){             // configure seconds
        setCursor(0,0);
        writeLCD("Set seconds:", 12);
        time = getConfMinSec();                     // get configured seconds
        setSeconds(time);                           // set configured minutes
    }
    setCursor(1,0);
    char timeStr[2];
    sprintf(timeStr, "%2d", time);                  // create value string of hour, minutes or seconds
    writeLCD(timeStr, 2);                           // write string on display
}

/* method to change state of menu */
void changeState(u8 oldState, u8 newState){
    if(oldState == MENU_LIVE_DATA && newState == MENU_TIME_CONF){
        stopTime();                                 // to prevent writing while clock changes
    }
    else if(oldState == MENU_TIME_CONF && newState == MENU_LIVE_DATA){
        startTime();                                // start RTCC again
    }
}


int main(int argc, char** argv) {
    setup();
    while(1){
        // check if menu state changed
        u8 newState = checkS1Button(menuState);
        changeState(menuState, newState);
        menuState = newState;
        
        // display selected menu
        if(menuState == MENU_LIVE_DATA){
            showLiveData();
        }
        if(menuState == MENU_TIME_CONF){
            showTimeMenu();
        }
    }
    return (EXIT_SUCCESS);
}

