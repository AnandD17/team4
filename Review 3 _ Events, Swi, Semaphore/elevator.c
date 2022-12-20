#include <rtl.h>
#include <lpc21xx.h>

void delay(unsigned int);
void serial(void);
void input(void);
void display_cur(void);
__task void job1 (void);
__task void job2 (void);
void extint0_ISR(void)__irq;
void extint1_ISR(void)__irq;
void __swi(8) eint0_init(void);
void __swi(9) eint1_init(void);


unsigned char cur_floor[] = {"\nCurrent Floor : "};  //17
unsigned char ip[] = {"\nEnter the floor : "}; //19
unsigned char elev[4] = {'0','1','2','3'};
unsigned int row0[4]={ 0x00ee0000,0x00ed0000,0x00eb0000,0x00e70000};
int i, value, dest, cur, x, flag;
OS_TID t1, t2;
OS_RESULT RE1, RE2;
OS_SEM sem1;

int main()
{
	serial();
	eint0_init();
	eint1_init();
	IODIR0=0xf0f00000;
	IOSET0=0x20000000;
	IOCLR0=0x00f00000;
	cur = 0;
	display_cur();
	os_sys_init(job1);
}

__task void job1 (void)
{
	t1 = os_tsk_self();
	t2 = os_tsk_create(job2,0);
	while(1)
	{
		os_sem_wait (sem1, 0xff);
		RE1 = os_evt_wait_and(0x0005, 0xfffe);	  // wait for event
		if(RE1 == OS_R_EVT)
		{
			while(!(U0LSR & 0x20));
			U0THR = '\n';
			switch(x)
			{
				case 1: {
									for(i=cur; i>=dest; i--)
									{
										while(!(U0LSR & 0x20));
										U0THR = elev[i];
										while(!(U0LSR & 0x20));
										U0THR = '\n';
										delay(1000000);
										delay(1000000);
										delay(1000000);
									}
									cur = dest;
									//display_cur();
									break;
				        }
				case 2: {
									for(i=cur; i<=dest; i++)
									{
										while(!(U0LSR & 0x20));
										U0THR = elev[i];
										while(!(U0LSR & 0x20));
										U0THR = '\n';
										delay(1000000);
										delay(1000000);
										delay(1000000);
									}
									cur = dest;
									//display_cur();
									break;
								}
			}
			while(!(U0LSR & 0x20));
			U0THR = '\n';
			os_sem_send(sem1);
		}
	}
}

__task void job2(void)
{
	while(1)
	{
		RE2 = os_sem_wait(sem1, 0xfffe);
		if(RE2 == OS_R_SEM)
		{
			display_cur();
			delay(1000000);
			os_sem_send(sem1);
		}
	}
}

void __SWI_8(void) 
{
	PINSEL1 |= 0x000000001;
	EXTMODE = 0x00000001;
	VICVectAddr0 = (unsigned long)extint0_ISR;
	VICVectCntl0 = 0x20|14;
	VICIntEnable |= 0x00004000;
}

void __SWI_9(void)
{
	PINSEL0 |= 0x000000c0; //P0.3 - 11
	EXTMODE = 0x00000001;
	VICVectAddr1 = (unsigned long)extint1_ISR;
	VICVectCntl1 = 0x20|15; //VICIntENABLE - 15
	VICIntEnable |= 0x00008000; //eNABLE - P0.15
}	

void extint0_ISR(void)__irq
{
	if(cur!=0)
	{
		input();
		if(dest<=cur)
			x = 1;
		//else
		//x = 0;
		isr_evt_set(0x0005,t1);
	}
	EXTINT|=0X00000001; //Clear interrupt flag
	VICVectAddr=0; // End of interrupt execution
}

void extint1_ISR(void)__irq
{
	if(cur!=3)
	{
		input();
		if(dest>=cur)
			x = 2;
		//else
		//	x = 0;
		isr_evt_set(0x0005,t1);
	}
	EXTINT |= 0X00000002 ; //Clear interrupt flag
	VICVectAddr=0; // End of interrupt execution
}

void input()
{
	for(i=0; i<19; i++)
	{
		while(!(U0LSR & 0x20));
		U0THR = ip[i];
		delay(1000);
	}
	while(!(U0LSR & 0x01));
	dest = U0RBR;
	while(!(U0LSR & 0x20));
	U0THR = dest;
	if(dest=='0')
		dest = 0;
	else if(dest>='4')
		input();
	else
		dest = dest - 48;					
}

void serial()
{
  PINSEL0 |= 0x00000005; 			
  U0LCR = 0x83; 					
  U0DLL = 0x61; 					
  U0LCR = 0x03; 					
  U0IER = 0x01;						
}

void display_cur()
{
	for(i=0; i<17; i++)
	{
		while(!(U0LSR & 0x20));
		U0THR = cur_floor[i];
	  delay(10000);
	}
	while(!(U0LSR & 0x20));
		U0THR = elev[cur];
	while(!(U0LSR & 0x20));
	U0THR = '\n';
}

void delay(unsigned int del)
{
	unsigned int q;
	for(q=0; q<del; q++); 
}