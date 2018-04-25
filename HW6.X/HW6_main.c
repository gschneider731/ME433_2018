#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>
#include<stdio.h>
#include "ST7735.h"

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
#define LCD LATBbits.LATB15

#define CS LATAbits.LATA0       // chip select pin

void drawChar(unsigned short x,unsigned short y,char mess,unsigned short colorOn,unsigned short colorOff)
{
    int col; char pixels; int j;
    for(col=0;col<5;col++)
    {
        pixels = ASCII[mess-0x20][col];
        for(j=0;j<8;j++)
        {
            if((pixels>>j)&1)
            {
                LCD_drawPixel(x+col,y+j,colorOn);
            }
            else
            {
                LCD_drawPixel(x+col,y+j,colorOff);
            }
        }
    }
}

void drawString(unsigned short x,unsigned short y,char* message,unsigned short colorOn,unsigned short colorOff)
{
    int i = 0;
    while(message[i])
    {
        drawChar(x+(5*i),y,message[i],colorOn,colorOff);
        i++;
    }
}
void drawBarBackground(unsigned short x,unsigned short y,unsigned short length,unsigned short height,unsigned short color)
{
    unsigned short left; unsigned short right; unsigned short top; unsigned short bottom;
    left = x - 1;
    right = x + length + 1;
    top = y + 1;
    bottom = y - height - 1;
    int count;
    //draw top and bottom
    for(count=0;count<=(length+2);count++)
    {
        LCD_drawPixel(left+count, top, color);
        LCD_drawPixel(left+count, bottom, color);
    }
    //draw left and right
    for(count=0;count<=(height+2);count++)
    {
        LCD_drawPixel(left, top-count, color);
        LCD_drawPixel(right, top-count, color);
    }
}
void drawProgressBar(unsigned short progress,unsigned short x,unsigned short y,unsigned short length,unsigned short height,unsigned short color)
{
    int count;
    if(progress >=0 && progress <= length+1)
    {
        for(count=0;count<=(height);count++)
        {
            LCD_drawPixel(progress+x, y-count, color);
        }
    }
}

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

    SDI1Rbits.SDI1R = 0b0100; //define SDI1 as RPB8
    RPA1Rbits.RPA1R = 0b0011; //define SDO1 as RPA1
    RPB7Rbits.RPB7R = 0b0011; //define RPB7 as SS 
    
    __builtin_enable_interrupts();
    
//    //draw line
//    int p;
//    for(p=0;p<50;p++)
//    {
//        LCD_drawPixel(p, 5, WHITE);
//    }
    
    //draw character
//    drawChar(1,1,'A',WHITE,BLACK);
    
    //draw string
//    char message[30];
//    sprintf(message,"Hello");
//    drawString(1,1,message,WHITE,BLACK);
  
    //create background for bar
    unsigned short length = 100; unsigned short height = 10;
    unsigned short barx = 14; unsigned short bary = 60;
    drawBarBackground(barx,bary,length,height,WHITE);
   // drawProgressBar(50,barx,bary,length,height,RED);
    
    //show progress to 100
    int count;
    char message[30];
    for(count=0;count<=length;count++)
    {
        _CP0_SET_COUNT(0);
        sprintf(message,"Hello World %d",count);
        drawString(28,32,message,WHITE,BLACK);
        drawProgressBar(count,barx,bary,length,height,RED);
        while (_CP0_GET_COUNT() < 1200000)
        {
           
        }
    }
    
    while(1) 
  {
        
  }
  return 0;
}


/* *****************************************************************************
 End of File
 */
