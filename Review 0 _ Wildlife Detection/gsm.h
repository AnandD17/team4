#include<lpc21xx.h>
#include<stdio.h>

unsigned int i;
unsigned char init[] = {"AT\r\n"}; //4
unsigned char txt_mode[] = {"AT+CMGF=1\r\n"}; // 11
unsigned char txt_no[] = {"AT+CMGS=\"9481519484\"\r\n"}; //24
unsigned char gsm_msg[] = {"Wildlife Detected!!"}; //19

void sendMessage()
{
	for(i=0;i<4;i++)
	{
		while(!(U0LSR & 0x20));
		U0THR = init[i];
	}
	for(i=0;i<11;i++)
	{
		while(!(U0LSR & 0x20));
		U0THR = txt_mode[i];
	}
	for(i=0;i<24;i++)
	{
		while(!(U0LSR & 0x20));
		U0THR = txt_no[i];
	}
	delay(100);
	for(i=0;i<19;i++)
	{
		while(!(U0LSR & 0x20));
		U0THR = gsm_msg[i];
	}
	while(!(U0LSR & 0x20));
	U0THR = 0x1A; //CTRL+Z
	delay(1000);
}