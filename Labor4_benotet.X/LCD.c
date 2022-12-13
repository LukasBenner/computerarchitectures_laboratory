
/* some useful functions for controlling LCD */

#define I2C_ADDRESS             0x7C    // 0x3E << 1
#include <xc.h>

typedef unsigned char u8;
typedef unsigned int u32;

/* BEGIN FORWARD DECLARATIONS */
void delay_us(unsigned int us);
/* END FORWARD DECLARATIONS */

/* method sets init configurations of I2C */
void initI2C(){
    ANSELBbits.ANSB13 = 0;
    I2C3CON = 0;
    TRISBbits.TRISB7 = 0;         // RB7 (SDA) as output
    TRISBbits.TRISB13 = 0;        // RB13 (SCL) as output
    I2C3BRGbits.I2CBRG = 0x1C;    // 28
    I2C3CONbits.A10M = 0;         // 7 bit address
    I2C3CONbits.DISSLW = 0;       // high speed mode (400kHz)
    
    I2C3CONbits.ON = 1;           // start I2C
}

/* method to start I2C transmission */
void startI2C(){
    I2C3CONbits.SEN = 1;               // enable I2C transmission
    while(I2C3CONbits.SEN == 1);
}

/* method to transmit data */
u8 writeI2C(u8 data){
    I2C3TRN = data;                     // set data to transmit register
    while(I2C3STATbits.TRSTAT == 1);    // wait until status changes
    u8 ack = 1 - I2C3STATbits.ACKSTAT;  // invert ack status for logic conversion
    return ack;
}

/* method to stop I2C transmission */
void stopI2C(){
    I2C3CONbits.PEN = 1;                // disable I2C transmission
    while(I2C3CONbits.PEN == 1);
}

/* method to send message over I2C */
u8 sendI2C(u8 instr, u8 value) {
    u8 ack;
	
    startI2C();
    // transmit adress
    ack = writeI2C(I2C_ADDRESS);
    // send instr and value only if address was acknowledged
    if (ack == 1) {
        ack = writeI2C(instr);
	ack = writeI2C(value);
    }
    stopI2C();
    delay_us(30); 		// delay required after each instruction
    return ack;
}

/* method to init LCD configurations */
void initLCD() {
    u8 mode = 0x00;      // mode for writing a single instruction
    delay_us(50000);     // additional delay after power-on required
    sendI2C(mode,0x38);  // 8 bit interface, 2 lines, set basic table
    sendI2C(mode,0x39);  // set extended table
    sendI2C(mode,0x14);  // adjust internal oscillator frequency
    sendI2C(mode,0x74);  // set contrast (low part)
    sendI2C(mode,0x54);  // set contrast (high part), enable booster for 3,3V
    sendI2C(mode,0x6F);  // follower control 
    sendI2C(mode,0x0E);  // display on, show cursor
    sendI2C(mode,0x01);  // clear display
    delay_us(1200);      // delay required for clearing display
}

/* method to clear LCD display */
void clearLCD() {
    u8 mode = 0x00;
	sendI2C(mode,0x01);  // clear display
	delay_us(1200);      // required for clearing display
}

/* method to write string of specific lengtg to LCD display */
void writeLCD (char* str, u32 len) {
	u8 mode = 0x40;
	u32 i;
	if (len > 0) {
        // send each character
        for (i=0; i<len; i++)
            sendI2C(mode,str[i]);
    }
}

/* method to set cursor on display*/
void setCursor(u8 row, u8 col) {
	u8 p;
	u8 mode = 0x80;
	
	// set cursor to row 0 or 1 and column 0 to 15
	if (row > 1) row = 0;
	if (col > 15) col = 0;
	if (row == 0) 
		p = 0x80 + col;
	else 
		p = 0xC0 + col;
	sendI2C(mode, (0x00 | p));
}

