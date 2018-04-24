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

#define addr 0b0100000

unsigned char readi2c()
{
    i2c_master_start();
    i2c_master_send(addr<<1|0);
    i2c_master_send(0x09);
    i2c_master_restart();
    i2c_master_send(addr<<1|1);
    unsigned char r = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return r;
}

void writei2c(unsigned char reg, unsigned char val)
{
    i2c_master_start();
    i2c_master_send(addr<<1|0);
    i2c_master_send(reg);
    i2c_master_send(val);
    i2c_master_stop();
}

void initExpander()
{
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    //i2c_master_setup();
    
    writei2c(0x00,0b11110000);
    writei2c(0x0A,0b00001111);
}

void setExpander(unsigned char pin, unsigned char level)
{
    unsigned char val = 0b00000000;
    val = (val|level)<<pin;
    writei2c(0x09,val);
    
}

unsigned char getExpander()
{
    unsigned char val = readi2c();
    return val;
}

int main(void) {  
 
  //int_spi();
    initExpander();
  
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
    TRISBbits.TRISB4 = 1;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;

    
//    int count1 = 1;
//    int count2 = 1;
//    char vtrimax;
//    char vtri;
//    _CP0_SET_COUNT(0);
    //setExpander(1,0);
    writei2c(0x0A,1);
    unsigned char r;
    _CP0_SET_COUNT(0);
    int turn = 0;
    LATAbits.LATA4 = 0;
  while(1) {
      
      r = getExpander();
      r = r>>7; //pin 7
      if((r>>7) == 0)
      {
          //setExpander(1,0);
          writei2c(0x0A,1);
      }
      else
      {
          //setExpander(0,0);
          writei2c(0x0A,0);
      }    
      
      //blinking light
       //wait 0.5 ms and exit if button pressed
        if (_CP0_GET_COUNT() < 12000 && PORTBbits.RB4 == 1 && turn == 1)
        {
            //reset
            _CP0_SET_COUNT(0);
            if(turn = 0)
            {
                //turn off LED
                LATAbits.LATA4 = 1;
                turn = 1;
            }
            else
            {
                //turn on LED
                LATAbits.LATA4 = 0;
                turn = 0;
            }
           
        }
    
  }
  return 0;
}


/* *****************************************************************************
 End of File
 */
