#ifndef __TEST_MODULE_HPP__
#define __TEST_MODULE_HPP__

#include<stdint.h>
#include<systemc>

#include"libqemu.h"
#include"amba_pv.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace amba_pv;

class test_module: public sc_module, public amba_pv_slave_base<32>
{
public:
	amba_pv_slave_socket<32> bus_s;
public:
	sc_out<bool> a;
	sc_out<bool> b;
public:
	SC_HAS_PROCESS(test_module);
	test_module(sc_module_name name): amba_pv_slave_base<32>((const char*)name)
	{
		bus_s(*this);
	}
public:
	amba_pv_resp_t read(int id, const uint64& addr, unsigned char* data, unsigned int size, const amba_pv_control* ctrl, sc_time& delay)
	{
		amba_pv_resp_t ret = AMBA_PV_SLVERR;

		ret = AMBA_PV_OKAY;

		printf("read \n");

		return ret;
	}
	amba_pv_resp_t write(int id, const uint64& addr, unsigned char* data, unsigned int size, const amba_pv_control* ctrl, unsigned char* strb, sc_time& delay)
	{
		amba_pv_resp_t ret = AMBA_PV_SLVERR;

		ret = AMBA_PV_OKAY;

		printf("write\n");

		return ret;
	}
};

#endif//__TEST_MODULE_HPP__
