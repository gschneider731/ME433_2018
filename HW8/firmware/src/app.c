/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>
#include<stdio.h>
#include "i2c_master_noint.h"
#include "ST7735.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
    #define PB PORTBbits.RB4
    #define LED LATAbits.LATA4
    #define LCD LATBbits.LATB15
    #define CS LATAbits.LATA0       // chip select pin
    #define LSM 0b01101011

    LCD_init();
    LCD_clearScreen(BLACK);
    
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB15 = 0;

    i2c_master_setup();
    initIMU(LSM);
    
    drawBarBackground(64,111,40,2,WHITE);
    drawProgressBar(0,0,64,111,40,2,GREEN);
    
    
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            unsigned char r;
    
            int turn = 1;

            char message[30];
            char readdata[15];
            unsigned char whoami;

            signed short temperature; signed short gyroX; signed short gyroY;
            signed short gyroZ; signed short accelX;
            signed short accelY; signed short accelZ;

            float xacc; float yacc; float zacc;
            
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

            //scale 16000 to 40
            xacc = accelX/400;
            yacc = accelY/400;
            zacc = -accelZ/400;


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

            drawProgressBar(xacc,zacc,64,111,40,2,GREEN);

            LATAbits.LATA4=1;

            while(_CP0_GET_COUNT()<24000000/10)
            { 
            }

            _CP0_SET_COUNT(0);
            LATAbits.LATA4=0;
            while(_CP0_GET_COUNT()<24000000/10)
            { 
            }
   
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
