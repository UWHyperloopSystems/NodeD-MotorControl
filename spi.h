
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
void delay_milli(unsigned int time);
void spi_init();
unsigned int spi_data_ready();
char spi_read();
void spi_write(char message);
unsigned long spi_read_long();
char oneReadCycle(char num);
