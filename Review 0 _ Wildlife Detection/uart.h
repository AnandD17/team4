#include<lpc21xx.h>
#include<stdio.h>


void serial(void);
void displayUART(char[],unsigned char[],unsigned char[]);
unsigned int i;

void serial()
{
  PINSEL0 = 0x00000005; 			
  U0LCR = 0x83; 					
  U0DLL = 0x61; 					
  U0LCR = 0x03; 					
  U0IER = 0x01;						
}

void displayUART(char t[],unsigned char ms1[], unsigned char ms2[])
{
	for(i=0;i<8;i++)
	{
		while(!(U0LSR & 0x20));		
    U0THR = t[i];
	}
	U0THR='\n';
	for(i=0;i<8;i++)
	{
		while(!(U0LSR & 0x20));		
    U0THR = ms1[i];
	}
	for(i=0;i<10;i++)
	{
		while(!(U0LSR & 0x20));		
    U0THR = ms2[i];
	}
	U0THR='\n';
	while(!(U0LSR & 0x20));		
	U0THR='\n';
	delay(10000);
}