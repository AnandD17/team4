#include<lpc21xx.h>
#include<stdio.h>

void buzzer(void);
void dcMotor(void);

void buzzer()
{
	IOSET0 = 0x00004000;			 //Buzzer On
	delay(1000000);
	IOCLR0 = 0x00004000;			 //Buzzer Off
}

void dcMotor()									//Close the gates
{
	IOCLR0 = 0x00000c00;
	IOSET0 = 0x00000400;
	delay(1000000);
	IOCLR0 = 0x00000c00;
}