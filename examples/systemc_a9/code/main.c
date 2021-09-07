#include<stdint.h>

#define NULL ((void*)(0))

__attribute__((optimize("-O0")))
void delay(int n)
{
	static volatile int num = 0;

	for(int i=0;i<n;i++)
		for(int j=0;j<n;j++)
			num++;
}

int main(void)
{
	unsigned char i='A';
	int flag = 0;
	while(1)
	{
		delay(2);


		while(*((uint8_t*)0xFFFF0004))
		{
		}

		*((uint8_t*)0xFFFF0004) = i;

		i++;

		if(i > 'Z')
			i='A';
	}
	return 0;
}

void do_irq(void)
{
	//*((uint32_t*)0xFFFFFF00) = 0x1234;
}
void do_fiq(void)
{
}
