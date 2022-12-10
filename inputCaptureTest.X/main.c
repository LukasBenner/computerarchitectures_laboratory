/* 
 * File:   main.c
 * Author: lukas
 *
 * Created on December 10, 2022, 6:14 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

typedef unsigned char u8;
typedef unsigned int u32;

void SYSTEM_Initialize(void);


void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    
    
    CCP1CON1bits.CCSEL = 1; // Input capture mode
    CCP1CON1bits.CLKSEL = 0; // Set the clock source (Tcy)
    CCP1CON1bits.T32 = 0; // 16-bit Dual Timer mode
    CCP1CON1bits.MOD = 1; // Capture ever rising edge of the event
    CCP1CON2bits.ICS= 0; // Capture rising edge on the Pin
    CCP1CON1bits.OPS = 0; // Interrupt on every input capture event
    CCP1CON1bits.TMRPS = 0; // Set the clock prescaler (1:1)
    CCP1CON1bits.ON = 1; // Enable CCP/input capture
    
    
    IFS2bits.CCP1IF = 0;
    
}

int main(int argc, char** argv) {
    setup();
    
    while(1){
        if(IFS2bits.CCP1IF == 1){
            IFS2bits.CCP1IF = 0;
            TRISAbits.TRISA0 = 0;
            u32 val = CCP1BUF;
            val = val + 1;
        }
    }
    
    return (EXIT_SUCCESS);
}

