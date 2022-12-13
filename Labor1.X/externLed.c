
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/attribs.h>

void externLed(){
    
    /*
     * LED an Port A14
     * Button S1 an Port B9
     * LED mit 120 Ohm Vorwiederstand
     * 3,3V - 2,1V = 1,2V
     * R = U/I = 1,2V / 10mA = 120 Ohm
     */
    TRISBSET = 0b001000000000;    //Button on Port B9 as input
    TRISACLR = 0x4000;           //LED on Port A14 as output
    LATACLR = 0x4000;
    
    while(1){
        uint32_t button = PORTBbits.RB9;
        
        if(button == 0){  
            LATAINV = 0x4000;  //Toggle LED on Port A14
            while(button == 0){     //prevent multiple clicks in one press
                button = PORTBbits.RB9;
            }
            //button is 0 now
            //cycle can restart now
        }
    }
}