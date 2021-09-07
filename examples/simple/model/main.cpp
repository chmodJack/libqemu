#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<thread>
#include<unistd.h>

#include"libqemu.h"

using namespace std;

void sync(void)
{
	//called every instruction cycle
}
int memory_read(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs)
{
	printf("read : addr: %x, size: %d\n", addr, size);
	return 0;
}
int memory_write(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs)
{
	printf("write: addr: %x, size: %d, data:", addr, size);
	for(int i=0;i<size;i++)
		printf(" %x", ((uint8_t*)data)[i], ((uint8_t*)data)[i], ((uint8_t*)data)[i]);
	printf("\n");

	return 0;
}
void signal_output(int n, int level)
{
}

void test_func(void)
{
	uint64_t old = 0;
	while(1)
	{
		sleep(1);
		printf("libqemu_icount: %lu, %lu\n",libqemu_icount - old, libqemu_icount);
		old = libqemu_icount;
	}
}

int main(int argc,char* argv[])
{
	libqemu_extern_sync = sync;
	libqemu_extern_memory_read = memory_read;
	libqemu_extern_memory_write = memory_write;
	libqemu_extern_signal_output = signal_output;

    thread t(test_func);
    t.detach();

	const char* _argv[] = 
	{
		"libqemu",

		"-nographic",
		"-serial",
		"null",
		"-monitor",
		"none",

		"-icount",
		"1",
		"-singlestep",
		"-d",
		"nochain",

		"-M",
		"libqemu-arm-cortex-a9",
		"-cpu",
		"cortex-a9",
		"-smp",
		"1",
		"-m",
		"64M",

		"-S",
		"-gdb",
		"tcp::1234",

		"-kernel",
		"../code/main.elf",
	};
	int _argc = sizeof(_argv) / sizeof(_argv[0]);
	char* _envp[] = { };

	qemu_init(_argc,(char**)_argv,_envp);
	qemu_main_loop();
	qemu_cleanup();

	return 0;
}

