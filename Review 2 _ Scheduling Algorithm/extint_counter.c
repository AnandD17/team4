#include <rtl.h>
#include <lpc21xx.h>

__task void job1(void);
__task void job2(void);
__task void job3(void);
void extint0_ISR(void)__irq;
void serial(void);
void delay(unsigned int);

OS_TID t1, t2, t3;
int p1=1, p2=0, p3 = 0;
unsigned int i, x, count = 0;
char c[2];

int main()
{
	serial();
	PINSEL1 |= 0x00000001;    // EINT0 P0.16 - 01
	EXTMODE = 0x00000001;
	VICVectAddr0 = (unsigned long)extint0_ISR;
	VICVectCntl0 = 0x20|14;
	VICIntEnable |= 0x00004000;
	os_sys_init(job1);
}

void serial()
{
  PINSEL0 |= 0x00000005; 			
  U0LCR = 0x83; 					
  U0DLL = 0x61; 					
  U0LCR = 0x03; 					
  U0IER = 0x01;						
}

void extint0_ISR(void)__irq
{
	//Updating Priorities
	count++;
	p2++;
	p1 = 0;
	p3 = 0;
	EXTINT|=0X00000001; //Clear interrupt flag
	VICVectAddr=0; // End of interrupt execution
}

__task void job1(void)
{
	t2 = os_tsk_create(job2,p2);
	t3 = os_tsk_create(job3,p3);
	t1 = os_tsk_self();
	while(1)
	{
		os_tsk_prio(t1,p1);
		os_tsk_prio(t3,p3);
		os_tsk_prio(t2,p2);
	}
}

__task void job2(void)
{
	while(1)
	{
		sprintf(c,"%d",count);
		for(i=0; i<2; i++)
		{
			while(!(U0LSR & 0x20));
	   	U0THR = c[i];
			delay(10000);
		}
		while(!(U0LSR & 0x20));
	   	U0THR = '\t';
			delay(10000);
		p1 = 0;
		p2 = 0;
		p3++;
		os_tsk_prio(t2,p2);
		os_tsk_prio(t1,p1);
		os_tsk_prio(t3,p3);
	}
}

__task void job3(void)
{
	IODIR0 = 0x00004000;  //Buzzer (P0.14)
	while(1)
	{
		if(count%2==0)
			x = 2;
		else
			x = 1;
		for(i=0; i<x; i++)
		{
			IOSET0 = 0x00004000; //Buzzer On
			delay(1000000);
			IOCLR0 = 0x00004000; //Buzzer Off
			delay(1000000);
		}
		while(!(U0LSR & 0x20));
	  U0THR = '\n';
		delay(1000);
		while(!(U0LSR & 0x20));
	  U0THR = '\r';
		while(!(U0LSR & 0x20));
	  U0THR = '\r';
		delay(10000);
		p3 = 0;
		p2 = 0;
		p1++;
		os_tsk_prio(t2,p2);
		os_tsk_prio(t3,p3);
		os_tsk_prio(t1,p1);
	}
}

void delay(unsigned int del)
{
  unsigned int z;
  for(z=0;z<del;z++);
}