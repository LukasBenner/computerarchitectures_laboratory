/* 
 * File:   main.c
 * Author: lukas
 *
 * Created on November 14, 2022, 11:00 AM
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <xc.h>

void SYSTEM_Initialize(void);
void initDisplay();

void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    initDisplay();
}

void mod10Counter();
void twoDigitCounter();
void digitalThermometer();
void digitalInterruptThermometer();

int main(void) {
    setup();    
    mod10Counter();
    //twoDigitCounter();
    //digitalThermometer();
    //digitalInterruptThermometer();
}