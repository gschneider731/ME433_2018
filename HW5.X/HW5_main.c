/* ************************************************************************** */
/** Descriptive File Name

  @Name
    Grant Schneider

  @File Name
    HW4_main.c

 */
/* ************************************************************************** */

//#include "NU32.h"       // constants, funcs for startup and UART
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>
#include "i2c_master_noint.h"

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

#define PB PORTBbits.RB4
#define LED LATAbits.LATA4

#define CS LATAbits.LATA0       // chip select pin

#define addr 0b00100001

int main(void) {  
//// 
////  //int_spi();
////    
////  
////  __builtin_disable_interrupts();
////
////    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
////    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
////
////    // 0 data RAM access wait states
////    BMXCONbits.BMXWSDRM = 0x0;
////
////    // enable multi vector interrupts
////    INTCONbits.MVEC = 0x1;
////
////    // disable JTAG to get pins back
////    DDPCONbits.JTAGEN = 0;
////
////    // do your TRIS and LAT commands here
////    TRISAbits.TRISA4 = 0;
////    TRISBbits.TRISB4 = 1;
////    TRISBbits.TRISB2 = 0;
////    TRISBbits.TRISB3 = 0;
////
////    i2c_master_setup();
////    initExpander();
////    unsigned char r;
////    _CP0_SET_COUNT(0);
////    int turn = 1;
////    LATAbits.LATA4 = 1;
////    
////    __builtin_enable_interrupts();
////  while(1) {
////            
////      r = getExpander();
////     
////      
////      //r = r>>7; //pin 7
////      
////      if((r>>7) == 0)
////      {
////          setExpander(1,0);
////      }
////      else
////      {
////          setExpander(0,0);
////      }    
////      
////      
////      //blinking light
//////        if (_CP0_GET_COUNT() < 120000000 && turn == 1)
//////        {
//////            //reset
//////            _CP0_SET_COUNT(0);
//////            if(turn = 0)
//////            {
//////                //turn off LED
//////                LATAbits.LATA4 = 0;
//////                turn = 1;
//////            }
//////            else
//////            {
//////                //turn on LED
//////                LATAbits.LATA4 = 1;
//////                turn = 0;
//////            }
//////           
//////        }
////    
////  }
////  return 0;
          
    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0;
    
    //TRISBbits.TRISB4 = 1;
    
    //__builtin_enable_interrupts();
    
    //init_spi();
    i2c_master_setup();
    initExpander(); //
    float count1=1;
    float count2=1;
    float v1=0;
    float v2=0;
    float pi=3.121;
    float steps = 100.0;
    unsigned char val=0;
    int aux = 0;
    
    __builtin_enable_interrupts();
    
    while(1) 
    {
        _CP0_SET_COUNT(0);
//        count2=count2+1.0;
//        v1=floor(512.0*(cos(2.0*pi*count2/steps)+1));
//            setVoltage(0,v1);
//        if (count2>2*steps)
//        {
//            count1=0;
//            //v1=0.0;
//            
//        }
//        //count2=count2+1;
//        if (count2==2*steps)
//        {
//            setVoltage(1,v2);
//            count2=0;
//            v2=0;
//            
//        }   
//        if (count2<steps)
//        {
//            v2=1024.0*count2/steps;
//            setVoltage(1,v2);
//        }
//        if (count2>steps)
//        {
//            v2=-1024.0*(count2-steps)/steps;
//            setVoltage(1,v2);
//        }
        
        val = getExpander();
        aux = (val>>7); //look at pin 0 only
        if (aux){ // if button is pressed
            setExpander(1,0); // level (high), pin number (GP0)
        }
        else {
            setExpander(0,0);
        }
        
        LATAbits.LATA4=1;
            
        while(_CP0_GET_COUNT()<24000000/10){ }
        
        _CP0_SET_COUNT(0);
        LATAbits.LATA4=0;
        while(_CP0_GET_COUNT()<24000000/10){ }
    }          

}


/* *****************************************************************************
 End of File
 */
