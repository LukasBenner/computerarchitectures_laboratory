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

void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
	TRISD &= 0b0111 ;     // set bit 3 of Port D for output
}

void loop() { 
	int i;
	while(1) {
		LATDSET = 0b1000;       // set bit 3 of Port D
		for (i=0; i< 1000000; i++);
        
        LATDCLR = 0b1000;       // clear bit 3 of Port D
        for (i=0; i< 1000000; i++); 
  }
}

int main(void) {
    setup();
    loop();
}