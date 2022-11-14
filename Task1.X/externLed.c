
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
     * */
    TRISBSET = 0b01000000000;    //Button on Port B9 as input
    TRISACLR = 0x4000;           //LED on Port A14 as output
    
    while(1){
        uint32_t portB_value = PORTB;
        uint8_t button = (portB_value & 0b0100000000) >> 9;
        
        if(button == 1){
            
            LATAINV = 0x4000;  //Toggle LED on Port A14
            
            while(button == 1){     //prevent multiple clicks in one press
                portB_value = PORTB;
                button = (portB_value & 0b0100000000) >> 9;
            }
            //button is 0 now
            //cycle can restart now
        }
    }
}