#include<lpc21xx.h>
#include<stdio.h>

void cmd(unsigned int);
void data(unsigned int);
void displayLCD(unsigned char[], unsigned char[]);
unsigned int i;

void displayLCD(unsigned char ms1[],unsigned char ms2[])
{
			cmd(0x80);
			delay(10000);
			for(i=0;i<8;i++)
			{
				data(ms1[i]);
				delay(1000);
			} 
			cmd(0xc0);
			delay(10000);
			for(i=0;i<10;i++)
			{
				data(ms2[i]);
				delay(1000);
			}  
			delay(6500000);
			delay(6500000);
			delay(650000);
			cmd(0x01);
			delay(65000);
			delay(65000);
			delay(65000);
			delay(65000);
}

void cmd(unsigned int value)
{
	unsigned int y;
	y=value;
	y=y & 0xf0;
	IOCLR0=0x000000fc;
	IOCLR0=0X00000004;	//rs=0
	IOSET0=y;
	IOSET0=0x00000008;	//en=1
	delay(10);
	IOCLR0=0x00000008;	//en=0
	y=value;
	y=y & 0x0f;
	y=y<<4;
	IOCLR0=0x000000fc;
	IOCLR0=0X00000004;	//rs=0
	IOSET0=y; 
	IOSET0=0x00000008;	//en=1
	delay(10);
	IOCLR0=0x00000008;	//en=0
}

void data(unsigned int dat)
{
	unsigned int y;
	y=dat;
	y=y & 0xf0;
	IOCLR0=0x000000fc;
	IOSET0=0X00000004;	//rs=1
	IOSET0=y; 
	IOSET0=0x00000008;	//en=1
	delay(10);
	IOCLR0=0x00000008;	//en=0
	y=dat;
	y=y & 0x0f;
	y=y<<4;
	IOCLR0=0x000000fc;	 
	IOSET0=0X00000004;	//rs=1
	IOSET0=y;
	IOSET0=0x00000008;	//en=1
	delay(10);
	IOCLR0=0x00000008;	//en=0
}