#include <rtl.h>
#include <lpc21xx.h>
#include <math.h>

void delay(unsigned int);
void serial(void);
__task void job1 (void);
__task void job2 (void);
__task void job3 (void);
void __swi(8) zero_division(void);
void __swi(9) float_operation(void);
void __swi(10) subtraction_error(void);

OS_TID t1, t2, t3;
unsigned int i;
int a, b, c;
float d, e, f, g, h;
unsigned char err1[] = {"\nERROR - Division by 0\n"}; //23
unsigned char err2[] = {"\nERROR - Float Operation\n"}; //25
unsigned char err3[] = {"\nERROR - Negative result\n"}; //25

int main()
{
	serial();
	os_sys_init(job1);
}

__task void job1(void)
{
	t2 = os_tsk_create(job2,0);
	t3 = os_tsk_create(job3,0);
	t1 = os_tsk_self();
	while(1)
	{
		a = 10;
	  b = 0;
	  c = 0;
		if(b==0)
			zero_division();
	  else
			c = a/b;
		delay(2000000);
	}
}

__task void job2(void)
{
	d = 10.322;
	e = 19;
	f = floor(d);
	g = floor(e);
	while(1)
	{
		if((d-f > 0) || (e-g)>0)
			float_operation();
		else
			h = d + e;
	  delay(2000000);
	}
}

__task void job3(void)
{
	while(1)
	{
		a = 23;
	  b = 25;
	  c = 0;
		if(b>a)
			subtraction_error();
		else
			c = a-b;
		delay(2000000);
	}
}

void __SWI_8(void) 
{
	for(i=0;i<23;i++)
	{
		while(!(U0LSR & 0x20));
		U0THR = err1[i];
		delay(10000);
	}
}

void __SWI_9(void) 
{
	for(i=0;i<25;i++)
	{
		while(!(U0LSR & 0x20));
		U0THR = err2[i];
		delay(10000);
	}
}

void __SWI_10(void) 
{
	for(i=0;i<25;i++)
	{
		while(!(U0LSR & 0x20));
		U0THR = err3[i];
		delay(10000);
	}
}

void serial()
{
  PINSEL0 |= 0x00000005; 			
  U0LCR = 0x83; 					
  U0DLL = 0x61; 					
  U0LCR = 0x03; 					
  U0IER = 0x01;						
}

void delay(unsigned int del)
{
	unsigned int q;
	for(q=0; q<del; q++); 
}
