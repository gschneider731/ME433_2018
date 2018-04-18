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

//#define LDAC LATBbits.LATB7 = 1;
#define SHDN LATBbits.LATB9 = 0;

#define CS LATAbits.LATA0       // chip select pin

void int_spi()
{
    TRISAbits.TRISA0 = 0;
  CS = 1;
  
   //not sure where these go
  RPB15Rbits.RPB15R = 0b0011; //define SS1 as RPB15
  RPB13Rbits.RPB13R = 0b0011; //define SDO1 as RPB13
  SDI1Rbits.SDI1R = 0b0000; //define SDI1 as RPA1

  // setup spi1
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 0x1000;            // baud rate to 10 MHz [SPI4BRG = (80000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi 4
    
    
}

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void setVoltage(char channel, int voltage)
{
    unsigned char a = 0b0000000000000000;
    //bit shift channel to bit 15 (total 16 bits cuz bit 0)
    a |= channel << 15;
    //bit shift buffer bit
    a |= 0b1 << 14;
    //bit shift gain bit
    a |= 0b1 << 13;
    //bit shift SHDN bit
    a |= 0b1 << 12;
    //bit shift voltage (next 10 bits)
    a |= voltage << 2;
    
    CS = 0;
    
    a = spi_io(a>>10);
    a = spi_io(a);
    
    CS = 1;
    
}

int main(void) {
//  unsigned short addr1 = 0x1234;                  // the address for writing the ram
//  char data[] = "Help, I'm stuck in the RAM!";    // the test message
//  char read[] = "***************************";    // buffer for reading from ram
//  char buf[100];                                  // buffer for comm. with the user
//  unsigned char status;                           // used to verify we set the status 
//  NU32_Startup();   // cache on, interrupts on, LED/button init, UART init
//  ram_init(); 
  
  
 
  int_spi();
  
  
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

    
    int count = 1;
    char vtrimax;
    char vtri;
  while(1) {
    //set voltage function
    _CP0_SET_COUNT(0);
    
    //setVoltage(1,512);
    //setVoltage(0,512/2);
    //float f = 512 + 512 
    
    while(_CP0_GET_COUNT()<24000000/1000)
    {
        
    }
    
    char vsine = floor(512*sin(((2*3.14*10)*count)+1));
    
    if(count <= 500)
    {
        vtri = floor(512 * count/1000);
        vtrimax = vtri;
    }
    if(count > 500)
    {
        vtri = floor(vtrimax - (512 * (count-500)/1000));
    }
    
    setVoltage(1,vsine);
    setVoltage(0,vtri);
    
    if(count>1000)
    {
        count = 0;
        _CP0_SET_COUNT() = 0;
    }
    
    
  }
  return 0;
}


/* *****************************************************************************
 End of File
 */
