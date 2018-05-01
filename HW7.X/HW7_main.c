/* ************************************************************************** */
/** Descriptive File Name

  @Name
    Grant Schneider

  @File Name
    HW7_main.c

 */
/* ************************************************************************** */

//#include "NU32.h"       // constants, funcs for startup and UART
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>
#include<stdio.h>
#include "ST7735.h"
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
#pragma config USERID = 00001111 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

#define PB PORTBbits.RB4
#define LED LATAbits.LATA4

#define LCD LATBbits.LATB15

#define CS LATAbits.LATA0       // chip select pin

//#define addr 0b00100001//0b00100000
#define LSM 0b01101011

int main(void) {  
    
  __builtin_disable_interrupts();
  LCD_init();
  LCD_clearScreen(BLACK);

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
    TRISBbits.TRISB15 = 0;

//    SDI1Rbits.SDI1R = 0b0100; //define SDI1 as RPB8
//    RPA1Rbits.RPA1R = 0b0011; //define SDO1 as RPA1
//    RPB7Rbits.RPB7R = 0b0011; //define RPB7 as SS 

    i2c_master_setup();
    //initExpander(addr);
    initIMU(LSM);
    unsigned char r;
    
    int turn = 1;
    LATAbits.LATA4 = 1;
    
    char message[30];
    char readdata[15];
    unsigned char whoami;
    
    signed short temperature; signed short gyroX; signed short gyroY;
    signed short gyroZ; signed short accelX;
    signed short accelY; signed short accelZ;
    
    float xacc; float yacc; float zacc;
    
    
    __builtin_enable_interrupts();
    
    
    
  while(1) {
    _CP0_SET_COUNT(0);
   
    
    whoami = readi2c(LSM,0x0F);
    sprintf(message,"WHO AM I: %d",whoami);
    drawString(1,1,message,WHITE,BLACK);
    
    i2cMultipleRead(LSM, 0X20, readdata);

    
    temperature=(readdata[1]<<8)|readdata[0];
    gyroX=(readdata[3]<<8)|readdata[2];
    gyroY=(readdata[5]<<8)|readdata[4];
    gyroZ=(readdata[7]<<8)|readdata[6];
    accelX=(readdata[9]<<8)|readdata[8];
    accelY=(readdata[11]<<8)|readdata[10];
    accelZ=(readdata[13]<<8)|readdata[12];
    
    //scale 16000 to 60
    xacc = accelX/267;
    yacc = accelY/267;
    zacc = accelZ/267;
    
    
    sprintf(message,"TEMPERATURE: %d",temperature);
    drawString(1,9,message,WHITE,BLACK);
    sprintf(message,"GYROX: %d",gyroX);
    drawString(1,17,message,WHITE,BLACK);
    sprintf(message,"GYROY: %d",gyroY);
    drawString(1,25,message,WHITE,BLACK);
    sprintf(message,"GYROZ: %d",gyroZ);
    drawString(1,33,message,WHITE,BLACK);
    sprintf(message,"ACCELX: %3.1f",xacc);
    drawString(1,41,message,WHITE,BLACK);
    sprintf(message,"ACCELY: %3.1f",yacc);
    drawString(1,49,message,WHITE,BLACK);
    sprintf(message,"ACCELZ: %3.1f",zacc);
    drawString(1,57,message,WHITE,BLACK);
    
    LATAbits.LATA4=1;
            
    while(_CP0_GET_COUNT()<24000000/10){ }
        
    _CP0_SET_COUNT(0);
    LATAbits.LATA4=0;
    while(_CP0_GET_COUNT()<24000000/10){ }
   
  }
  return 0;
}


/* *****************************************************************************
 End of File
 */
