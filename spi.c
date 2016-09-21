/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>
#include <pic18f4685.h>        /* For true/false definition */
#include "spi.h"

void spi_init()
{
    TRISCbits.RC4 = 1;          // SDI as input
    TRISCbits.RC5 = 0;          // SDO as output
    TRISCbits.RC3 = 0;          // SCK/SCL as output (cleared)
    
    TRISCbits.TRISC6 = 0;   //CSG
    TRISCbits.TRISC7 = 0;   //CSXM
    
    SSPCON1bits.SSPEN = 0;      // Clear bit to reset, SPI pins
    
    SSPCON1bits.CKP = 1;        // Set polarity to idle at 1
    SSPSTATbits.CKE = 0;        // Set transmit on rising edge
    SSPSTATbits.SMP = 0;        // Set Sample bit to 0
    SSPCON1bits.SSPM0 = 0;      // Set SSPM[3:0] as stated in manual
    SSPCON1bits.SSPM1 = 0;      // In order to divide by 4 (Fosc/4)
    SSPCON1bits.SSPM2 = 0;
    SSPCON1bits.SSPM3 = 0;
    SSPCON1bits.WCOL = 0;       // Clear the write buffer
    
    SSPCON1bits.SSPEN = 1;      // Set bit to re enable SPI
       
}

unsigned int spi_data_ready()
{
    return SSPSTATbits.BF;
}


char spi_read()
{
    while(!SSPSTATbits.BF);
    return SSPBUF;
}


void spi_write(char message)
{
    //while(SSPCON1bits.WCOL);
    SSPBUF = message;
}

unsigned long spi_read_long()
{
    unsigned long total = 0;
    for(int i = 0; i < 4; i++)
    {
        total += spi_read() << (i * 8);
    }
    return total;
}

char oneReadCycle(char num)
{
    delay_milli(30);
    spi_write(num);                       // Send command to slave
    char measure;
    delay_milli(30);
    if(spi_data_ready())
    {
        measure = spi_read();
    }
    return measure; 
}

void delay_milli(unsigned int time)
{
    for (int i = 0; i < time; i++)
        __delay_ms(1);
}
