#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <sys/attribs.h>

typedef unsigned char u8;
typedef unsigned int u32;

/* BEGIN FORWARD DECLARATIONS */
void writeLCD (char* str, u32 len);
void setCursor(u8 row, u8 col);
/* END FORWARD DECLARATIONS */

/* method calculates bar and displays it */
void showBar(u32 distance){
    setCursor(1,0);
    char bar[16] = {' '};                      // set all 16 indexes to spaces -> 16 "fields" on display per row
    u32 upperLimit = 64;                       // max distance = 64
    if (distance < upperLimit)
        upperLimit = distance;
    upperLimit = 64 - upperLimit;              // turn around, to get complete bar at 0cm
    for(int i = 0; i< upperLimit; i+=4){
        u32 index = i / 4;                     // calculate index (64/16 = 4), every 4cm grows bar
        bar[index] = '>';
    }
    writeLCD(bar, 16);                         // display bar on LCD
}
