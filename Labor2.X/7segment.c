
/*
	Control a 7-segment display 
	at Pins A8 to A14 of port A
*/

#include <xc.h>

typedef unsigned char u8;
typedef unsigned int u32;

// Define the LED digit patters, from 0 - 9
// 1 = LED on, 0 = LED off, in this order:
// Segment A, B, C, D, E, F, G
u8  seven_seg_digits[10][7] = { 
   { 1,1,1,1,1,1,0 },  // = 0
   { 0,1,1,0,0,0,0 },  // = 1
   { 1,1,0,1,1,0,1 },  // = 2
   { 1,1,1,1,0,0,1 },  // = 3
   { 0,1,1,0,0,1,1 },  // = 4
   { 1,0,1,1,0,1,1 },  // = 5
   { 1,0,1,1,1,1,1 },  // = 6
   { 1,1,1,0,0,0,0 },  // = 7
   { 1,1,1,1,1,1,1 },  // = 8
   { 1,1,1,0,0,1,1 }   // = 9
};

void initDisplay() {               
   TRISACLR = 0b1111111100000000;  	// set pins for 7 segments and dot to output
   TRISBCLR = 0b0110;				// set bits 1 and 2 of port B to output
}

void writeDisplay(u32 segment, u8 bit) {
  if (bit == 1)
    LATASET = 0b100000000 << segment;
  else
    LATACLR = 0b100000000 << segment;
}
 
 void writeDot(u32 bit) {
  if (bit == 1)
    LATASET = 0b100000000 << 8;
  else
    LATACLR = 0b100000000 << 8;
}
   
void sevenSegWrite(u32 digit) {
   u32 segment;
   for (segment = 0; segment < 7; segment++) {
     writeDisplay(segment, seven_seg_digits[digit][segment]);
   }
}
