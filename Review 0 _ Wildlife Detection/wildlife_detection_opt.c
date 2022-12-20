#include<lpc21xx.h>
#include<stdio.h>
#include "del.h"
#include "lcd.h"
#include "uart.h"
#include "buzz_mo.h"
#include "rtc_ext_int.h"
#include "gsm.h"

void extint0_ISR(void)__irq;

unsigned char msg1[]={"Wildlife"};
unsigned char msg2[]={"Detected!!"};
unsigned int i, value, flag;
unsigned int w,x,y,z;
char time[10];

int main()
{
	//unsigned int x, y;
	unsigned int c[]={0x30,0x30,0x20,0x20,0x28,0x01,0x06,0x0e,0x80};
	PINSEL1=0x00000001;
	EXTMODE=0x00000001;
	VICVectAddr0=(unsigned long)extint0_ISR;
	VICVectCntl0=0x20|14;
	VICIntEnable|=0x00004000;
	serial();
	rtc();
	IODIR0=0x000f4cfc;	//LED(P0.16 to P0.19), Buzzer(P0.14), DC Motor(P0.10 & P0.11) and LCD
	IOCLR0=0x000000fc;
	
	for(i=0;i<9;i++)
	{
		cmd(c[i]);
		delay(10000);
	}
	while(1)
	{
		x = SEC;
		y = MIN;
		z = HOUR;
		w = 0x00100000 & IOPIN1;
		sprintf(time,"%d:%d:%d",z,y,x);
		IODIR1 = 0X02000000;
		if(HOUR>=6)												//If time is greater than or equal to 6, Turn on Lights(LED)
			//IOCLR0 = 0x000c0000;
			IOSET1 = 0X02000000;
		else
			//IOSET0 = 0x000c0000;
		  IOCLR1 = 0X02000000;
		if(w == 0x00100000)								//Open the gate
		{
			IOCLR0 = 0x00000c00;
			IOSET0 = 0x00000800;
			delay(1000000);
			IOCLR0 = 0x00000c00;
		}
	}
}

void extint0_ISR(void)__irq
{
	flag = 0;
	for(i=0;i<20000;i++)
	{
		flag = 0;
		value = IOPIN0 & 0x00800000;
		if(value==0x00800000)
		{
			flag = 1;
			break;
		}
	}
	if(flag==1)
	{
		buzzer();
		sendMessage();
		//displayUART(time,msg1,msg2);
		dcMotor();
		displayLCD(msg1,msg2);
		flag = 0;
		delay(100000);
	}
	EXTINT|=0X00000001;					//Clear interrupt flag
	VICVectAddr=0;							// End of interrupt execution
}