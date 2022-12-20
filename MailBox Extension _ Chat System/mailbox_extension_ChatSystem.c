/*
		CHAT SYSTEM USING MAILBOX
		
		D.S.Sanjaya - 01FE20BEC115
		Nitish D K  - 01FE20BEC111
		Murari G    - 01FE20BEC084
		Anand D     - 01FE20BEC102
*/
#include <rtl.h>
#include <lpc21xx.h>

void delay(unsigned int);
__task void job1(void);
__task void job2(void);
void serial_0(void);
void serial_1(void);

OS_TID t1, t2;
int p1=0, p2=0;
os_mbx_declare (MsgBox, 100);                /* Declare an RTX mailbox */
os_mbx_declare (MsgBox1, 100);                /* Declare an RTX mailbox */
_declare_box(mpool,20,32);	 /* Reserve a memory for 32 blocks of 20 bytes  */
int value;
unsigned int i;
unsigned char msg1[100], msg2[100]; // m1[100], m2[100];
unsigned char m[] = {"\r\nYou : "}; //8
unsigned char m1[] = {"\r\nUART0 : "}; //10
unsigned char m2[] = {"\r\nUART1 : "}; //10

int main()
{
	serial_0();
	serial_1();
	_init_box (mpool, sizeof(mpool), sizeof(U32));
	os_sys_init(job1);
}

__task void job1(void)
{
	U32 *mptr;
	U32 *rptr;
	os_mbx_init(MsgBox, sizeof(MsgBox));
  mptr = _alloc_box(mpool);                /* Allocate a memory for the message */
	t1 = os_tsk_self();
	t2 = os_tsk_create(job2,0);
	while(1)
	{
		os_mbx_wait (MsgBox1, (void*)&rptr, 0x00ff);      /* Wait for the message to arrive. */
		while(!(U0LSR & 0x20));
      U0THR = '\n';
		while(!(U0LSR & 0x20));
      U0THR = '\r';
		for(i=0; i<10; i++)
		{
			while(!(U0LSR & 0x20));
      U0THR = m2[i];
		}
		i=0;
		while((rptr[i]!='\0'))
		{
			while(!(U0LSR & 0x20));
			U0THR = rptr[i];
			i++;
		}
		for(i=0; i<8; i++)
		{
			while(!(U0LSR & 0x20));
      U0THR = m[i];
		}
		i=0;
		while(msg1[i-1] != '.')
		{
			while(!(U0LSR & 0x01));
			msg1[i] = U0RBR;
			mptr[i] = msg1[i];
			while(!(U0LSR & 0x20));
      U0THR = mptr[i];
			i++;
			delay(100);
		}
		mptr[i] = '\0';
		delay(10000);
		os_mbx_send (MsgBox, mptr, 0xffff);
		p2++;
		p1 = 0;
		os_tsk_prio(t1,p1);
		os_tsk_prio(t2,p2);
	}
}

__task void job2(void)
{
	U32 *mptr;
	U32 *rptr;
	os_mbx_init(MsgBox1, sizeof(MsgBox1));
  mptr = _alloc_box(mpool);                /* Allocate a memory for the message */
	while(1)
	{
		os_mbx_wait (MsgBox, (void*)&rptr, 0xffff);      /* Wait for the message to arrive. */
		while(!(U1LSR & 0x20));
    U1THR = '\n';
		while(!(U1LSR & 0x20));
    U1THR = '\r';
		for(i=0; i<10; i++)
		{
			while(!(U1LSR & 0x20));
      U1THR = m1[i];
		}
		i=0;
		while(rptr[i]!='\0')
		{
			while(!(U1LSR & 0x20));
      U1THR = rptr[i];
			i++;
		}
		for(i=0; i<8; i++)
		{
			while(!(U1LSR & 0x20));
      U1THR = m[i];
		}
		i=0;
		while(msg2[i-1] != '.')
		{
			while(!(U1LSR & 0x01));
			msg2[i] = U1RBR;
			mptr[i] = msg2[i];
			while(!(U1LSR & 0x20));
      U1THR = mptr[i];
			i++;
			delay(100);
		}
		mptr[i] = '\0';
		delay(10000);
		os_mbx_send (MsgBox1, mptr, 0x00ff);
		p1++;
		p2 = 0;
		os_tsk_prio(t2,p2);
		os_tsk_prio(t1,p1);
	}
}

void serial_0()
{
  PINSEL0 |= 0x00000005;
  U0LCR = 0x83;
  U0DLL = 0x61;
  U0LCR = 0x03;
  U0IER = 0x01;
}

void serial_1()
{
  PINSEL0 |= 0x00050000; 			
  U1LCR = 0x83; 					
  U1DLL = 0x61; 					
  U1LCR = 0x03; 					
  U1IER = 0x01;						
}


void delay(unsigned int del)
{
	unsigned int qq;
	for(qq=0;qq<del;qq++);
}