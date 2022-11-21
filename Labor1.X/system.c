
// 'C' source line config statements

// FDEVOPT
#pragma config SOSCHP = OFF             // Secondary Oscillator High Power Enable bit (SOSC oprerates in normal power mode.)
#pragma config ALTI2C = ON              // Alternate I2C1 Pins Location Enable bit (Alternate I2C1 pins are used)
#pragma config FUSBIDIO = ON            // USBID pin control (USBID pin is controlled by the port function)
#pragma config FVBUSIO = ON             // VBUS Pin Control (VBUS pin is controlled by port function)
#pragma config USERID = 0xFFFF          // User ID bits (User ID bits)

// FICD
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)
#pragma config ICS = PGx2               // ICE/ICD Communication Channel Selection bits (Communicate on PGEC2/PGED2)

// FPOR
#pragma config BOREN = BOR3             // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware; SBOREN bit disabled)
#pragma config RETVR = OFF              // Retention Voltage Regulator Enable bit (Retention regulator is disabled)
#pragma config LPBOREN = ON             // Downside Voltage Protection Enable bit (Low power BOR is enabled, when main BOR is disabled)

// FWDT
#pragma config SWDTPS = PS1048576       // Sleep Mode Watchdog Timer Postscale Selection bits (1:1048576)
#pragma config FWDTWINSZ = PS25_0       // Watchdog Timer Window Size bits (Watchdog timer window size is 25%)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Watchdog timer is in non-window mode)
#pragma config RWDTPS = PS1048576       // Run Mode Watchdog Timer Postscale Selection bits (1:1048576)
#pragma config RCLKSEL = LPRC           // Run Mode Watchdog Timer Clock Source Selection bits (Clock source is LPRC (same as for sleep mode))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (WDT is disabled)

// FOSCSEL
#pragma config FNOSC = FRCDIV           // Oscillator Selection bits (Fast RC oscillator (FRC) with divide-by-N)
#pragma config PLLSRC = FRC             // System PLL Input Clock Selection bit (FRC oscillator is selected as PLL reference input on device reset)
#pragma config SOSCEN = ON              // Secondary Oscillator Enable bit (Secondary oscillator (SOSC) is enabled)
#pragma config IESO = OFF               // Two Speed Startup Enable bit (Two speed startup is disabled)
#pragma config POSCMOD = OFF            // Primary Oscillator Selection bit (Primary oscillator is disabled)
#pragma config OSCIOFNC = OFF           // System Clock on CLKO Pin Enable bit (OSCO pin operates as a normal I/O)
#pragma config SOSCSEL = OFF            // Secondary Oscillator External Clock Enable bit (Crystal is used (RA4 and RB4 are controlled by SOSC))
#pragma config FCKSM = CSECME           // Clock Switching and Fail-Safe Clock Monitor Enable bits (Clock switching is enabled; Fail-safe clock monitor is enabled)

// FSEC
#pragma config CP = OFF                 // Code Protection Enable bit (Code protection is disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/attribs.h>

#define SYS_FREQ 24000000

void SYSTEM_Initialize(void)
{
    volatile uint32_t i;
    static bool initialized = false;
    
    if(initialized == false)
    {
        initialized = true;

        //Unlock sequence for SPLLCON and OSCCON registers.
        SYSKEY = 0;// force lock
        SYSKEY = 0xAA996655;  // unlock sequence
        SYSKEY = 0x556699AA;  // lock sequence

        //Temporarily switch to 8MHz FRC (without PLL), so we can safely change the PLL settings,
        //in case we had previously been already running from the PLL.
        OSCCON = OSCCON & 0xF8FFF87E;    //FRC configured for 8MHz output, and set NOSC to run from FRC with divider but without PLL.
        if(OSCCONbits.COSC != OSCCONbits.NOSC)
        {
            //Initiate clock switching operation.
            OSCCONbits.OSWEN = 1;
            while(OSCCONbits.OSWEN == 1);    //Wait for switching complete.
        }

        //Configure the PLL to run from the FRC, and output 24MHz for the CPU + Peripheral Bus (and 48MHz for the USB module)
        SPLLCON = 0x02050080;     //PLLODIV = /4, PLLMULT = 12x, PLL source = FRC, so: 8MHz FRC * 12x / 4 = 24MHz CPU and peripheral bus frequency.

        //Now switch to the PLL source.
        OSCCON = OSCCON | 0x00000101;    //NOSC = SPLL, initiate clock switch (OSWEN = 1)

        //Wait for PLL startup/lock and clock switching operation to complete.
        for(i = 0; i < 100000; i++)
        {
            if((CLKSTATbits.SPLLRDY == 1) && (OSCCONbits.OSWEN == 0))
                break;
        }    

        SYSKEY = 0;             //Re-lock oscillator registers  

        INTCONbits.MVEC = 1;
        asm volatile("ei $0");
    }
}

void delay_us(unsigned int us) {
    us *= SYS_FREQ/1000000/2;
    _CP0_SET_COUNT(0);        //set core timer to zero
    while (us > _CP0_GET_COUNT()) {};
}
