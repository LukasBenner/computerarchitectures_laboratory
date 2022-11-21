#include <stdint.h>
#include <xc.h>

typedef unsigned int u32;

void sevenSegWrite();
void delay_us();

void mod10Counter(){
    u32 counter = 0;
    while(1){
        sevenSegWrite(counter);
        counter ++;
        counter = counter % 10;
        delay_us(1000000);      // 1Hz
    }
}