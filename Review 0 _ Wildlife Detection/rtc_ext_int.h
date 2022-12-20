#include<lpc21xx.h>
#include<stdio.h>

void rtc(void);

void rtc(void)
{
	PREINT=0X5A;
	PREFRAC=0X46C0;
	CCR=0X00;
	SEC=30;
	MIN=59;
	HOUR=5;
	CCR=0X01;
}