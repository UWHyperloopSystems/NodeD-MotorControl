/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <stdio.h> 

#endif

#include "can.h"
#include "uart.h"
#include "i2c.h"
#include "user.h"
#include "config.h"
#include "interrupts.h"
#include "ADC_Config.h"    /* ADC Channel selects and read function */


/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */
// Adjust to system clock
#define _XTAL_FREQ 16000000
unsigned int potFeedback = 0;
unsigned int wantedPosition = 10;

/******************************************************************************/
/* Function Prototypes                                                        */
/******************************************************************************/
//void print_can_message(Message* mess);

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
Message data; //global message used to send can signals

void main(void)
{
    /* Initialization functions called below */
    ConfigureOscillator();
    ecan_init();
    setAnalogIn();
     
    TRISAbits.TRISA0 = 0;    // set pin digital 0 as an output
    TRISAbits.TRISA6 = 0;
    TRISAbits.TRISA7 = 0;
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    int wait = 10;

    while(1)
    {  
        LATCbits.LATC1 ^= 1;
        selectAN4();                //7th pin down on left side
        int potFeedback = readADC();
        if(potFeedback < (wantedPosition + 4) && potFeedback > wantedPosition - 4){
            LATAbits.LATA6 = 0;
            LATAbits.LATA7 = 0; 
        } else if( potFeedback < wantedPosition){
            LATAbits.LATA6 = 1;
            LATAbits.LATA7 = 0;
        } else{
            LATAbits.LATA6 = 0;
            LATAbits.LATA7 = 1;
        } 
        
        wait_ms(wait);
        if(RXB0CONbits.RXB0FUL){
            LATCbits.LATC0 ^= 1;
            Message newMessage;
            ecan_receive_rxb0(&newMessage);

            char data0 = newMessage.data[0];

            if(data0 == 0x02){
                LATAbits.LATA0 ^= 1;
                data.sid = 0x001;
                data.len = 1;
                data.data[0] = 0x32;            //code this so that when we send it 0x02 it will send back pot position
                ecan_send(&data);  
            }                    

            if (data0 == 'e')
            {
                wantedPosition = 860;
            }
            else if (data0 == 'r')
            {
                wantedPosition = 10;
            }
            else if (data0 == 'm')
            {
                wantedPosition = 512;
            }
            else if (data0 == '+') {
                wantedPosition++;
            }
            else if (data0 == '-') {
                wantedPosition--;
            }

            ecan_rxb0_clear();          // Clear flag
        }        
    }
}

