#ifndef __UART_HPP__
#define __UART_HPP__

#include<stdint.h>
#include<systemc>

#include"libqemu.h"
#include"amba_pv.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace amba_pv;

class uart: public sc_module, public amba_pv_slave_base<32>
{
public:
	amba_pv_slave_socket<32> bus_s;
public:
	sc_in<bool> rx;
	sc_out<bool> tx;
	//sc_out<bool> irq;
public:
	sc_event tx_event;
	sc_time baud_cycle{1/(float)115200, SC_SEC};
	uint8_t data_len = 8;
	uint8_t parity_check = 0;
	float stop_len = 1;
	uint16_t transmit_data = 0;
	bool transmit_busy = false;
	void (*transmit_callback)(void) = nullptr;
public:
	SC_HAS_PROCESS(uart);
	uart(sc_module_name name): amba_pv_slave_base<32>((const char*)name)
	{
		bus_s(*this);

		SC_THREAD(tx_thread);
		sensitive << tx_event;

		SC_THREAD(rx_thread);
		sensitive << rx.neg();
	}
	void end_of_elaboration()
	{
		tx = 1;
	}
public:
	//API
	void set_transmit_baud(uint32_t baud)
	{
		baud_cycle = sc_time(1 / (float)baud, SC_SEC);
	}
	//API
	void set_transmit_format(uint8_t data_len, uint8_t parity_check, float stop_len)
	{
		this->data_len = data_len;
		this->parity_check = parity_check;
		this->stop_len = stop_len;
	}
	//API
	void set_transmit_data(uint32_t data)
	{
		transmit_data = data;
	}
	//API
	void set_transmit_start(void)
	{
		tx_event.notify();
	}
	//API
	bool get_transmit_busy(void)
	{
		return transmit_busy;
	}
	//API
	void set_transmit_callback(void(*func)(void))
	{
		transmit_callback = func;
	}
private:
	void tx_thread(void)
	{
		while(1)
		{
			wait();

			transmit_busy = true;

			//start bit
			tx = 0;
			wait(baud_cycle);

			//transmit data
			int count = 0;
			for(int i=0;i<data_len;i++)
			{
				bool value = transmit_data & (1 << i);
				tx = value;

				if(value)
					count += 1;

				wait(baud_cycle);
			}

			if(parity_check)
			{
				//odd
				if(parity_check == 1)
				{
					if(count % 2)
						tx = 0;
					else
						tx = 1;
				}
				//even
				else if(parity_check == 2)
				{
					if(count % 2)
						tx = 1;
					else
						tx = 0;
				}
				else
				{
				}
			}

			tx = 1;
			wait(baud_cycle * stop_len);

			transmit_busy = false;

			if(transmit_callback)
				(*transmit_callback)();
		}
	}
	void rx_thread(void)
	{
		while(1)
		{
			wait();
		}
	}
public:
	amba_pv_resp_t read(int id, const uint64& addr, unsigned char* data, unsigned int size, const amba_pv_control* ctrl, sc_time& delay)
	{
		amba_pv_resp_t ret = AMBA_PV_SLVERR;

		*((uint32_t*)data) = 0;

		if(get_transmit_busy())
			*data = 1;
		else
			*data = 0;

		ret = AMBA_PV_OKAY;
		return ret;
	}
	amba_pv_resp_t write(int id, const uint64& addr, unsigned char* data, unsigned int size, const amba_pv_control* ctrl, unsigned char* strb, sc_time& delay)
	{
		amba_pv_resp_t ret = AMBA_PV_SLVERR;

		set_transmit_data(*data);
		set_transmit_start();

		ret = AMBA_PV_OKAY;
		return ret;
	}
};

#endif//__UART_HPP__
