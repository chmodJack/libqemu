#ifndef __CORTEX_A9_HPP__
#define __CORTEX_A9_HPP__

#include<stdint.h>
#include<systemc>

#include"libqemu.h"
#include"amba_pv.h"

using namespace std;
using namespace sc_core;
using namespace amba_pv;

class cortex_a9: public sc_module
{
public:
	amba_pv_master_socket<32> bus_m;
public:
	sc_in<bool> irq;
	sc_in<bool> fiq;
public:
	SC_HAS_PROCESS(cortex_a9);
	cortex_a9(sc_module_name name)
	{
		SC_METHOD(irq_handle);
		sensitive << irq;

		SC_METHOD(fiq_handle);
		sensitive << fiq;

		self = this;

		libqemu_extern_sync = cortex_a9::sync;
		libqemu_extern_memory_read = cortex_a9::memory_read;
		libqemu_extern_memory_write = cortex_a9::memory_write;
		libqemu_extern_signal_output = cortex_a9::signal_output;
	}
public:
	static cortex_a9* self;
public:
	static void sync(void)
	{
		sc_start(1, SC_US);
	}
	static int memory_read(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs)
	{
		//TODO: turn attrs to ctrl
		amba_pv_control ctrl;
		//TODO: quantum time, not implement
		sc_time delay = SC_ZERO_TIME;

		amba_pv_resp_t ret = self->bus_m.read(addr, (uint8_t*)data, size, &ctrl, delay);
		if(ret == AMBA_PV_OKAY)
			return 0;
		else
			return -1;
	}
	static int memory_write(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs)
	{
		//TODO: turn attrs to ctrl
		amba_pv_control ctrl;
		//TODO: quantum time, not implement
		sc_time delay = SC_ZERO_TIME;

		amba_pv_resp_t ret = self->bus_m.write(addr, (uint8_t*)data, size, &ctrl, nullptr, delay);
		if(ret == AMBA_PV_OKAY)
			return 0;
		else
			return -1;
	}
	static void signal_output(int n, int level)
	{
		//TODO: from qemu signal
		printf("no implement yet.\n");
	}
public:
	void irq_handle(void)
	{
		libqemu_extern_signal_input(0,irq);
	}
	void fiq_handle(void)
	{
		libqemu_extern_signal_input(1,fiq);
	}
};

cortex_a9* cortex_a9::self = nullptr;

#endif//__CORTEX_A9_HPP__

