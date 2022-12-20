#include<lpc214x.h>
#include<stdio.h>

#define WREN_SET 	0X06     // Set the write enable latch (enable write operations)
#define WREN_CMD 	0X02     // Write data to memory array beginning at selected address
#define WREN_RESET 	0X04   	 // Reset the write enable latch (disable write operations)
#define READ_SET 	0X03     // Read data from memory array beginning at selected address

#define MSB_ADDRESS 0X00   	// dont care
#define LSB_ADDRESS 0X00   	// 16 bit address instruction 

#define CHIP_SEL	0X00100000;

void extint0_ISR(void)__irq;		// Interrupt Service Routine
void nvrom_read(void);    			// Nvrom Read func
void nvrom_write(void);   			// Nvrom Write func
void uart0_init(void);   			  // UART0 initialisation
void spi1_init(void);     			// spi1 initialisation
void uart_display(char *, int n);
void delay(unsigned int);

char arr1[] = "\rDATA WRITTEN INTO EEPROM\r" ;
char arr2[] = "STORED DATA = \r" ;
unsigned int count = 0;
unsigned int i = 0;
unsigned char dummy = 0x00, tem_var = 0x00, tem = 0x00;
char c[2];

int main()
{
	IO0DIR = CHIP_SEL; 	  		// P0.20 made as output.
	IO0SET = CHIP_SEL;  		  // chip select dissable( P0.20 made high). 
	spi1_init();
	uart0_init();
	PINSEL1 |= 0x00000001;    // EINT0 P0.16 - 01
	EXTMODE = 0x00000001;
	VICVectAddr0 = (unsigned long)extint0_ISR;
	VICVectCntl0 = 0x20|14;
	VICIntEnable |= 0x00004000;
	while(1);
}

void extint0_ISR(void)__irq
{
	//Main Code
	count++;
	nvrom_write();
	delay(10000);
	EXTINT|=0X00000001; //Clear interrupt flag
	VICVectAddr=0; // End of interrupt execution
}

void uart0_init()
{
  PINSEL0 |= 0x00000005; 	 // select TXD0 and RXD0 lines  OF UART0		
  U0LCR = 0x83; 					
  U0DLL = 0x61; 					
  U0LCR = 0x03; 					
  U0IER = 0x01;						
}

void spi1_init(void)
{
	PCONP |= (1<<10);				 //Enable the peripheral SPI1
  PINSEL1 &= ~0x000000FC;	 //P0.17 - SCK. P0.18 - MISO1. P0.19 - MOSI1
  PINSEL1 |= 0x000000A8;

  SSPCR0 = 0x707;	 			// 8 bit transfer , SPI , CPOL=CPHA=0, PCLK / (CPSDVSR * [SCR+1]) ,SCR=7
  SSPCR1 = 0x02;				//  enable SSP in master mode
  SSPICR = 0x01;				// clear intr
  SSPCPSR = 0x02;				// CPSDVSR = 2 ;
}

void nvrom_write()
{
  IO0CLR = CHIP_SEL ;	 					// Enabling SPI by setting CS line low.
  for(i=0; i<=1000; i++);  			// Delay
	
	while ( ((tem_var = SSPSR) & 0x01) == 0x00 );  // checking for transmit FIFO empty.
	SSPDR = WREN_SET; 							   						 // write enable
	
	while ( ((tem_var = SSPSR) & 0x10) == 0x10 );  // checking for SSP controller busy/idle.
	dummy = SSPDR;								   							 // dummy read to clear receive FIFO
  		   												
  IO0SET = CHIP_SEL;  				  //CS line should be  Toggle after write enable set. 
  for(i=0; i<=20000; i++);  			// Delay  
					  
  IO0CLR = CHIP_SEL;							
	
	while ( ((tem_var = SSPSR) & 0x01) == 0x00 );  // checking for transmit FIFO empty.
	SSPDR = WREN_CMD;    													 // write command.
	
	while ( ((tem_var = SSPSR) & 0x10) == 0x10 );  // checking for SSP controller busy/idle.
	dummy = SSPDR;																 // dummy read to clear receive FIFO
	
	while ( ((tem_var = SSPSR) & 0x01) == 0x00 );  // checking for transmit FIFO empty.
	SSPDR = MSB_ADDRESS; 													 // writing MSB of address

	while ( ((tem_var = SSPSR) & 0x10) == 0x10 );  // checking for SSP controller busy/idle.
	dummy = SSPDR;																 // dummy read to clear receive FIFO
	     
	while ( ((tem_var = SSPSR) & 0x01) == 0x00 );  // checking for transmit FIFO empty.
	SSPDR = LSB_ADDRESS;   												 // writing LSB of address
								
	while ( ((tem_var = SSPSR) & 0x10) == 0x10 );  // checking for SSP controller busy/idle.
	dummy = SSPDR;								    						 // dummy read to clear receive FIFO
  
	sprintf(c,"%d",count);
	delay(10000);
	for(i=0; i<2; i++)
	{						   
		while ( ((tem_var = SSPSR) & 0x01) == 0x00 );  // checking for transmit FIFO empty.
		SSPDR = c[i];																	 // writing data 
		delay(1000);
		while ( ((tem_var = SSPSR) & 0x10) == 0x10 );  // checking for SSP controller busy/idle.								   
		dummy = SSPDR;								   							 // doing dummy read	
		delay(10000);
	}
	
	//while ( ((tem_var = SSPSR) & 0x01) == 0x00 );
	//SSPDR = WREN_RESET;
	
	IO0SET = CHIP_SEL; 		 // dissabling SPI  by setting CS line HIGH. 
	
	uart_display(arr1, 26);
  delay(10000); 
	sprintf(c,"STORED DATA = %x\n",count);
	uart_display(c, 17);
	delay(10000);
	nvrom_read();
} // End of Nvrom_write

void nvrom_read()
{
	uart_display(arr2, 15);
  delay(10000);
	IO0CLR = CHIP_SEL;     	 		// Enabling SPI by setting CS line low.
	for( i = 0 ; i < 10000 ; i++ );	

	c[0] = c[1] = '0';
	while ( ((tem_var = SSPSR) & 0x01) == 0x00 ); 	 // checking for transmit FIFO empty. 
	SSPDR = READ_SET; 								 // write command.
	
	while ( ((tem_var = SSPSR) & 0x10) == 0x10 ); 	 // checking for SSP controller busy/idle.	
	dummy = SSPDR;									 // dummy read to clear receive FIFO
	
  	while ( ((tem_var = SSPSR) & 0x01) == 0x00 );	 // checking for transmit FIFO empty.
	SSPDR = MSB_ADDRESS;							 // writing MSB of address 

	while ( ((tem_var = SSPSR) & 0x10) == 0x10 ); 	 // checking for SSP controller busy/idle.
	dummy = SSPDR;									 // dummy read to clear receive FIFO
	
	while ( ((tem_var = SSPSR) & 0x01) == 0x00 ); 	// checking for transmit FIFO empty.
	SSPDR = LSB_ADDRESS;							// writing LSB of address

	while ( ((tem_var = SSPSR) & 0x10) == 0x10 ); 	 // checking for SSP controller busy/idle.
	dummy = SSPDR;									 // dummy read to clear receive FIFO
	
	/*while(((tem_var = SSPSR) & 0x04) == 0x04 )	   // doing dummy read till receive buffer gets empty.
		dummy = SSPDR; 	*/ 
		
		for(i=0; i<2; i++)
		{        
			delay(100);
			while(((tem_var = SSPSR) & 0x01) == 0x00 );  
			SSPDR = 0x00 ;									 // dummy write
		
			//while (((tem_var = SSPSR) & 0x10) == 0x10 );

			while(((tem_var = SSPSR) & 0x04) == 0x00 ); 
			c[i] = SSPDR;
			while(!(U0LSR & 0x20));		
      U0THR = c[i];
			delay(10000);
      //nvrom_rdata[i]= tem; 						 // data read
		}

    IO0SET = CHIP_SEL; 		  // dissabling SPI  by setting CS line HIGH. 

}// End of Nvrom_read

void uart_display(char *a, int n)
{
	for(i=0; i<n; i++)
	{
		while(!(U0LSR & 0x20));		
    U0THR = a[i];
		delay(10000);
	}
	delay(10000);
}

void delay(unsigned int x)
{
   unsigned int del;
   for(del=0;del<x;del++);
}

