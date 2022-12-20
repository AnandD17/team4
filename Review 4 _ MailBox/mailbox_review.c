#include <rtl.h>
#include <lpc21xx.h>

void delay(unsigned int);
__task void job1(void);
__task void job2(void);
void stepperMotorPause(void);
void stepperMotorAntiClockwise(void);
void stepperMotorClockwise(void);
void stop(void);

OS_TID t1, t2;
int p1=0, p2=0;
os_mbx_declare(MsgBox, 100);                /* Declare an RTX mailbox */
_declare_box(mpool,20,32);	 /* Reserve a memory for 32 blocks of 20 bytes  */
int value, msg, prev;

int main()
{
	IODIR0 = 0xf0004c00;  //Stepper Motor(P0.31 - P0.28) & Buzzer (P0.14)
	prev = 0;
	_init_box (mpool, sizeof(mpool), sizeof(U32));
	os_sys_init(job1);
}

__task void job1(void)
{
	U32 *mptr;
	os_mbx_init(MsgBox, sizeof(MsgBox));
  mptr = _alloc_box(mpool);                /* Allocate a memory for the message */
	t1 = os_tsk_self();
	t2 = os_tsk_create(job2,0);
	while(1)
	{
		value = IOPIN1;
		delay(100);
		value &= 0x00300000;
		if(value == 0x00000000)
		{
			mptr[0] = 0;
		}
		else if(value == 0x00100000)
		{
			mptr[0] = 1;
		}
		else if(value == 0x00200000)
		{
			mptr[0] = 2;
		}
		else if(value == 0x00300000)
		{
			mptr[0] = 3;
		}
		os_mbx_send (MsgBox, mptr, 0xffff);
		p2++;
		p1 = 0;
		os_tsk_prio(t1,p1);
		os_tsk_prio(t2,p2);
	}
}

__task void job2(void)
{
	U32 *rptr;
	while(1)
	{
		os_mbx_wait (MsgBox, (void**)&rptr, 0xffff);      /* Wait for the message to arrive. */
		msg = *rptr;
		switch(msg)
		{
			case 0: stepperMotorPause();
							prev = 0;
							break;
		  case 1: stepperMotorAntiClockwise();
							prev = 1;
							break;
			case 2: stepperMotorClockwise();
							prev = 1;
							break;
		  case 3: if(prev!=0)
								stop();
							break;
		}
		p1++;
		p2 = 0;
		os_tsk_prio(t2,p2);
		os_tsk_prio(t1,p1);
	}
}

void stepperMotorPause()
{
	IOCLR0 = 0xf0000000;
	delay(10000);
}

void stepperMotorAntiClockwise()
{
	IOSET0 = 0x80000000;
	delay(20000);
	IOCLR0 = 0x80000000;
	delay(20000);
	IOSET0 = 0x40000000;
	delay(20000);
	IOCLR0 = 0x40000000;
	delay(20000);
	IOSET0 = 0x20000000;
	delay(20000);
	IOCLR0 = 0x20000000;
	delay(20000);
	IOSET0 = 0x10000000;
	delay(20000);
	IOCLR0 = 0x10000000;
	delay(20000);
}

void stepperMotorClockwise()
{
	IOSET0 = 0x10000000;
	delay(20000);
	IOCLR0 = 0x10000000;
	delay(20000);
	IOSET0 = 0x20000000;
	delay(20000);
	IOCLR0 = 0x20000000;
	delay(20000);
	IOSET0 = 0x40000000;
	delay(20000);
	IOCLR0 = 0x40000000;
	delay(20000);
	IOSET0 = 0x80000000;
	delay(20000);
	IOCLR0 = 0x80000000;
	delay(20000);
}

void stop()
{
	IOSET0 = 0x00004000; //Buzzer On
	delay(1000000);
	IOCLR0 = 0x00004000; //Buzzer Off
	delay(1000000);
	os_tsk_delete(t1);
	os_tsk_delete(t2);
}

void delay(unsigned int del)
{
	unsigned int qq;
	for(qq=0;qq<del;qq++);
}