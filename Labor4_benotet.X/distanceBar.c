#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <sys/attribs.h>

typedef unsigned char u8;
typedef unsigned int u32;

void writeLCD (char* str, u32 len);
void setCursor(u8 row, u8 col);


void showBar(u32 distance){
    setCursor(1,0);
    char bar[16] = {' '};
    u32 upperLimit = 64;
    if (distance < upperLimit)
        upperLimit = distance;
    upperLimit = 64 - upperLimit;
    for(int i = 0; i< upperLimit; i+=4){
        u32 index = i / 4;
        bar[index] = '>';
    }
    writeLCD(bar, 16);
}