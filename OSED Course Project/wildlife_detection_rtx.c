#include <rtl.h>
#include <lpc21xx.h>
#include <stdio.h>

void cmd(unsigned int);
void data(unsigned int);
void delay(unsigned int);
void rtc(void);
void serial(void);
void buzzer(void);
void dcMotor(void);
void displayLCD(void);
void displayUART(void);
void extint0_ISR(void)__irq;
void extint2_ISR(void)__irq;

//Three Tasks
__task void job1(void);
__task void job2(void);
__task void job3(void);

//Two Software Interrupts 
void __swi(8) eint0_init(void);
void __swi(9) eint2_init(void);

//MailBox
os_mbx_declare(MsgBox, 100);                /* Declare an RTX mailbox */
_declare_box(mpool,20,32);	 /* Reserve a memory for 32 blocks of 20 bytes  */

OS_TID t1, t2, t3;
OS_RESULT RE1, RE2;
OS_SEM sem1;
unsigned char i, j, m1[10], m2[10];
unsigned int w,x,y,z;
char time[10];

int main()
{
	unsigned int c[]={0x30,0x30,0x20,0x20,0x28,0x01,0x06,0x0e,0x80};
	IODIR0=0x000c4cfc;	//LED(P0.16 to P0.19), Buzzer(P0.14), DC Motor(P0.10 & P0.11) and LCD
	serial();
	rtc();
	for(i=0;i<9;i++)
	{
		cmd(c[i]);
		delay(10000);
	}
	eint0_init();
	eint2_init();
	os_sem_init(sem1,0);
	_init_box (mpool, sizeof(mpool), sizeof(U32));
	os_sys_init(job1);
}

__task void job1(void)
{
	t1 = os_tsk_self();
	t2 = os_tsk_create(job2,1);
	t3 = os_tsk_create(job3,1);
	IODIR0=0x000f4cfc;	//LED(P0.16 to P0.19), Buzzer(P0.14), DC Motor(P0.10 & P0.11) and LCD
	IOCLR0=0x000000fc;
	while(1)
	{
		x = SEC;
		y = MIN;
		z = HOUR;
		sprintf(time,"%d:%d:%d",z,y,x);
		if(HOUR>=6)												//If time is greater than or equal to 6, Turn on Lights(LED)
			IOCLR0 = 0x000c0000;
		else
			IOSET0 = 0x000c0000;
		os_tsk_prio(t1,0);
		os_tsk_prio(t3,0);
		os_tsk_prio(t2,2);
	}
}

__task void job2(void)
{
	U32 *mptr;
	os_mbx_init(MsgBox, sizeof(MsgBox));
  mptr = _alloc_box(mpool);                /* Allocate a memory for the message */
	while(1)
	{
		unsigned char msg1[]={"Wildlife"};
		unsigned char msg2[]={"Detected!!"};
		RE1 = os_evt_wait_and(0x0005,0xfffd);
		RE2 = os_sem_wait(sem1, 0xff);
		if((RE1 == OS_R_EVT) && (RE2 == OS_R_SEM))
		{
			for(i=0;i<8;i++)
				mptr[i] = msg1[i];
			for(j=0; j<10; j++, i++)
				mptr[i] = msg2[j];
			os_mbx_send (MsgBox, mptr, 0xffff);
		}
		os_tsk_prio(t1,0);
		os_tsk_prio(t2,0);
		os_tsk_prio(t3,2);
	}
}

__task void job3(void)
{
	U32 *rptr;
	while(1)
	{
		os_mbx_wait (MsgBox, (void*)&rptr, 0xffff);      /* Wait for the message to arrive. */
		buzzer();
		for(i=0;i<8;i++)
				m1[i] = rptr[i];
		for(j=0; j<10; j++, i++)
				m2[j] = rptr[i];
		displayUART();
		displayLCD();
		dcMotor();
		delay(100000);
		os_tsk_prio(t2,0);
		os_tsk_prio(t3,0);
		os_tsk_prio(t1,2);
	}
}

void extint0_ISR(void)__irq
{
	isr_sem_send(sem1);
	EXTINT|=0X00000001; //Clear interrupt flag
	VICVectAddr=0; // End of interrupt execution
}

void extint2_ISR(void)__irq
{
	isr_evt_set(0x0005,t2);
	//isr_evt_set(0x0050,t2);
	EXTINT |= 0X00000004 ; //Clear interrupt flag
	VICVectAddr=0; // End of interrupt execution
}

void __SWI_8(void) 
{
	PINSEL1 |= 0x000000001;
	EXTMODE |= 0x00000001;
	VICVectAddr0 = (unsigned long)extint0_ISR;
	VICVectCntl0 = 0x20|14;
	VICIntEnable |= 0x00004000;
}

void __SWI_9(void)
{
	PINSEL0 |= 0x80000000; //P0.15 - 10 (31:30)
	EXTMODE |= 0x00000004;
	VICVectAddr2 = (unsigned long)extint2_ISR;
	VICVectCntl2 = 0x20|16; //VICIntENABLE - 16
	VICIntEnable |= 0x00010000; //ENABLE - 16
}	

void buzzer()
{
	IOSET0 = 0x00004000;			 //Buzzer On
	delay(1000000);
	IOCLR0 = 0x00004000;			 //Buzzer Off
}

void displayUART(void)
{
	for(i=0;i<8;i++)
	{
		while(!(U0LSR & 0x20));		
    U0THR = time[i];
	}
	U0THR='\n';
	for(i=0;i<8;i++)
	{
		while(!(U0LSR & 0x20));		
    U0THR = m1[i];
	}
	for(i=0;i<10;i++)
	{
		while(!(U0LSR & 0x20));		
    U0THR = m2[i];
	}
	U0THR='\n';
	while(!(U0LSR & 0x20));		
	U0THR='\n';
	delay(10000);
}

void displayLCD(void)
{
			cmd(0x80);
			delay(10000);
			for(i=0;i<8;i++)
			{
				data(m1[i]);
				delay(500);
			} 
			cmd(0xc0);
			delay(10000);
			for(i=0;i<10;i++)
			{
				data(m2[i]);
				delay(500);
			}  
			delay(650000);
			delay(650000);
			delay(650000);
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

void serial()
{
  PINSEL0 |= 0x00000005; 			
  U0LCR = 0x83; 					
  U0DLL = 0x61; 					
  U0LCR = 0x03; 					
  U0IER = 0x01;						
}

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

void dcMotor()
{
	IOCLR0 = 0x00000c00;
	IOSET0 = 0x00000400;
	delay(1000000);
	IOCLR0 = 0x00000c00;
}

void delay(unsigned int x)
{
   unsigned int del;
   for(del=0;del<x;del++);
}