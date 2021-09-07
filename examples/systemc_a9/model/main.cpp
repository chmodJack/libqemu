
#define SC_INCLUDE_DYNAMIC_PROCESSES
#include<systemc>

#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<thread>
#include<unistd.h>


#include"libqemu.h"

#include"cortex_a9.hpp"
#include"test_module.hpp"
#include"uart.hpp"

#include"./amba_pv_decoder_modified_version.hpp"


using namespace std;
using namespace sc_core;
using namespace sc_dt;

#if 0
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
#endif

int sc_main(int argc,char* argv[])
{
	printf("<thread: main, %s, %d>\n", __FILE__, __LINE__);

	_amba_pv::amba_pv_decoder bus("bus", 1, 2);

	cortex_a9 a9("a9");
	test_module tm("tm");
	uart u("u");


	a9.bus_m(bus.amba_pv_s[0]);
	bus.bind(0, tm.bus_s, 0xFFFF0000, 0xFFFF0003);
	bus.bind(1, u.bus_s, 0xFFFF0004, 0xFFFF0007);

	sc_signal<bool> irq;
	sc_signal<bool> fiq;

	sc_signal<bool> tx;
	sc_signal<bool> rx;

	a9.irq(irq);
	a9.fiq(fiq);

	tm.a(irq);
	tm.b(fiq);

	u.tx(tx);
	u.rx(rx);


	sc_trace_file* tf = sc_create_vcd_trace_file("main");
	sc_trace(tf, irq, "irq");
	sc_trace(tf, fiq, "fiq");
	sc_trace(tf, tx, "tx");
	sc_trace(tf, rx, "rx");

    //thread t(test_func);
    //t.detach();

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

 	sc_close_vcd_trace_file(tf);

	return 0;
}

