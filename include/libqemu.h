#ifndef __LIBQEMU_H__
#define __LIBQEMU_H__

#include<stdint.h>

extern "C"
{
	extern void qemu_init(int,char**,char**);
	extern void qemu_main_loop(void);
	extern void qemu_cleanup(void);

	//libqemu user API
	extern uint64_t libqemu_icount;
	extern void (*libqemu_extern_sync)(void);
	extern int (*libqemu_extern_memory_read)(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs);
	extern int (*libqemu_extern_memory_write)(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs);
	extern int libqemu_extern_read_memory(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs);
	extern int libqemu_extern_write_memory(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs);
	extern int libqemu_extern_read_memory_debug(uint64_t addr, uint64_t* data, uint64_t len);
	extern int libqemu_extern_write_memory_debug(uint64_t addr, uint64_t* data, uint64_t len);
	extern void libqemu_extern_signal_input(int n, int level);
	extern void (*libqemu_extern_signal_output)(int n, int level);
}

#endif//__LIBQEMU_H__
