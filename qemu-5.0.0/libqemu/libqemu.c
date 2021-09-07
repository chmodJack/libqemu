#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include"exec/memattrs.h"
#include"exec/hwaddr.h"

//qemu API
extern int64_t cpu_get_icount_raw(void);
extern MemTxResult address_space_read_full(void* as, hwaddr addr, MemTxAttrs attrs, void* buf, hwaddr len);
extern MemTxResult address_space_write(void* as, hwaddr addr, MemTxAttrs attrs, const void* buf, hwaddr len);
extern int cpu_memory_rw_debug(void* cpu, uint64_t addr, void* ptr, uint64_t len, int is_write);
extern void* qemu_get_cpu(int index);
extern void qemu_set_irq(void* irq, int level);


//libqemu user API
extern void (*libqemu_extern_sync)(void);
extern int (*libqemu_extern_memory_read)(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs);
extern int (*libqemu_extern_memory_write)(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs);
extern int libqemu_extern_read_memory(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs);
extern int libqemu_extern_write_memory(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs);
extern int libqemu_extern_read_memory_debug(uint64_t addr, uint64_t* data, uint64_t len);
extern int libqemu_extern_write_memory_debug(uint64_t addr, uint64_t* data, uint64_t len);
extern void libqemu_extern_signal_input(int n, int level);
extern void (*libqemu_extern_signal_output)(int n, int level);
extern uint64_t libqemu_icount;


//libqemu internal API
//MemTxResult:
//#define MEMTX_OK 0
//#define MEMTX_ERROR             (1U << 0) /* device returned an error */
//#define MEMTX_DECODE_ERROR      (1U << 1) /* nothing at that address */
extern void libqemu_sync(void);
extern int libqemu_memory_read(void* mr, hwaddr addr, uint64_t* data, unsigned int size, MemTxAttrs attrs);
extern int libqemu_memory_write(void* mr, hwaddr addr, uint64_t* data, unsigned int size, MemTxAttrs attrs);
extern int libqemu_read_memory(void* as, hwaddr addr, uint64_t* buf, unsigned int len, MemTxAttrs attrs);
extern int libqemu_write_memory(void* as, hwaddr addr, uint64_t* buf, unsigned int len, MemTxAttrs attrs);
extern int libqemu_read_memory_debug(void* cpu, uint64_t addr, void* ptr, uint64_t len);
extern int libqemu_write_memory_debug(void* cpu, uint64_t addr, void* ptr, uint64_t len);
extern void libqemu_signal_input(int n, int level);
extern void libqemu_signal_output(int n, int level);
extern void** libqemu_signal_array;


//libqemu user API
void (*libqemu_extern_sync)(void) = 0;
int (*libqemu_extern_memory_read)(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs) = 0;
int (*libqemu_extern_memory_write)(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs) = 0;
int libqemu_extern_read_memory(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs)
{
	MemTxAttrs _attrs;
	//TODO: trasn attrs to _attrs
	_attrs.unspecified = 1;

	return libqemu_read_memory(0, addr, data, size, _attrs);
}
int libqemu_extern_write_memory(uint64_t addr, uint64_t* data, uint64_t size, uint64_t attrs)
{
	MemTxAttrs _attrs;
	//TODO: trasn attrs to _attrs
	_attrs.unspecified = 1;

	return libqemu_write_memory(0, addr, data, size, _attrs);
}
int libqemu_extern_read_memory_debug(uint64_t addr, uint64_t* data, uint64_t len)
{
	return libqemu_read_memory_debug(0, addr, data, len);
}
int libqemu_extern_write_memory_debug(uint64_t addr, uint64_t* data, uint64_t len)
{
	return libqemu_write_memory_debug(0, addr, data, len);
}
void libqemu_extern_signal_input(int n, int level)
{
	libqemu_signal_input(n,level);
}
void (*libqemu_extern_signal_output)(int n, int level) = 0;


//libqemu internal API
uint64_t libqemu_icount = 0;
extern void qemu_mutex_lock_iothread_impl(const char *file, int line);
extern void qemu_mutex_unlock_iothread(void);
void libqemu_sync(void)
{
	int locked = 0;

	if(!qemu_mutex_iothread_locked())
	{
		qemu_mutex_lock_iothread_impl(__FILE__, __LINE__);
		locked = 1;
	}

	uint64_t icount = cpu_get_icount_raw();
	uint64_t increase = icount - libqemu_icount;
	libqemu_icount = icount;

	if(increase == 1)
	{
		(*libqemu_extern_sync)();
	}
	else if(increase == 0)
	{
	}
	else if(increase == 2)
	{
		(*libqemu_extern_sync)();
		(*libqemu_extern_sync)();
	}
	else
	{
		//printf("WARNNING: increase is not 0 or 1, [increase:%lx, icount:%lx], <%s, %d, %s>\n", increase, icount, __FILE__, __LINE__, __func__);
		(*libqemu_extern_sync)();
	}

	if(locked)
		qemu_mutex_unlock_iothread();
}
int libqemu_memory_read(void* mr, hwaddr addr, uint64_t* data, unsigned int size, MemTxAttrs attrs)
{
#if 0
		printf("libqemu_memory_read, mr:%p, addr:%lx, data:%lx, size:%x, attrs...\n", mr, addr, *data, size);
		return 0;
#else
		return (*libqemu_extern_memory_read)(addr, data, size, *((uint64_t*)(&attrs)));
#endif
}
int libqemu_memory_write(void* mr, hwaddr addr, uint64_t* data, unsigned int size, MemTxAttrs attrs)
{
#if 0
		printf("libqemu_memory_write, mr:%p, addr:%lx, data:%lx, size:%x, attrs...\n", mr, addr, *data, size);
		return 0;
#else
		return (*libqemu_extern_memory_write)(addr, data, size, *((uint64_t*)(&attrs)));
#endif
}
void* libqemu_address_space_memory;
void* libqemu_address_space_io;
int libqemu_read_memory(void* as, hwaddr addr, uint64_t* buf, unsigned int len, MemTxAttrs attrs)
{
#if 0
	if(as == 0)
		as = libqemu_address_space_memory;
	return address_space_read_full(as, addr, attrs, buf, len);
#else
	return address_space_read_full(libqemu_address_space_memory, addr, attrs, buf, len);
#endif
}
int libqemu_write_memory(void* as, hwaddr addr, uint64_t* buf, unsigned int len, MemTxAttrs attrs)
{
#if 0
	if(as == 0)
		as = libqemu_address_space_memory;
	return address_space_write(as, addr, attrs, buf, len);
#else
	return address_space_write(libqemu_address_space_memory, addr, attrs, buf, len);
#endif
}
int libqemu_read_memory_debug(void* cpu, uint64_t addr, void* ptr, uint64_t len)
{
#if 0
	if(cpu == 0)
		cpu = qemu_get_cpu(0);
	return cpu_memory_rw_debug(cpu, addr, ptr, len, 0);
#else
	return cpu_memory_rw_debug(qemu_get_cpu(0), addr, ptr, len, 0);
#endif
}
int libqemu_write_memory_debug(void* cpu, uint64_t addr, void* ptr, uint64_t len)
{
#if 0
	if(cpu == 0)
		cpu = qemu_get_cpu(0);
	return cpu_memory_rw_debug(cpu, addr, ptr, len, 1);
#else
	return cpu_memory_rw_debug(qemu_get_cpu(0), addr, ptr, len, 1);
#endif
}
//must be init in board init, point to qemu_irq signal array
void** libqemu_signal_array = 0;
void libqemu_signal_input(int n, int level)
{
	//LIBQEMU_NOTE:board implment qemu irq signal array
	qemu_set_irq(libqemu_signal_array[n],level);
}
void libqemu_signal_output(int n, int level)
{
#if 0
	if(libqemu_extern_signal_output)
		(*libqemu_extern_signal_output)(n,level);
#else
		(*libqemu_extern_signal_output)(n,level);
#endif
}


#if 0

//address_space_rw

typedef void (*qemu_irq_handler)(void* opaque, int n, int level);
struct IRQState
{
	Object parent_obj;
	qemu_irq_handler handler;
	void *opaque;
	int n;
};
typedef struct IRQState* qemu_irq;

qemu_irq signals[512];

void qemu_set_irq(qemu_irq irq, int level)
{
	if (irq)
		irq->handler(irq->opaque, irq->n, level);
}

qdev_get_gpio_in(cpu,ARM_CPU_IRQ);
sysbus_connect_irq(SYS_BUS_DEVICE(mpcore_priv), N, qdev_get_gpio_in(cpu, ARM_CPU_IRQ));
sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, usart_irq[i]));

mr = address_space_translate(as, addr, &addr1, &l, true, attrs);

/* virtual memory access for debug (includes writing to ROM) */
int cpu_memory_rw_debug(CPUState *cpu, target_ulong addr,
		void *ptr, target_ulong len, bool is_write)


	/* Every memory transaction has associated with it a set of
	 * attributes. Some of these are generic (such as the ID of
	 * the bus master); some are specific to a particular kind of
	 * bus (such as the ARM Secure/NonSecure bit). We define them
	 * all as non-overlapping bitfields in a single struct to avoid
	 * confusion if different parts of QEMU used the same bit for
	 * different semantics.
	 */
	typedef struct MemTxAttrs {
		/* Bus masters which don't specify any attributes will get this
		 * (via the MEMTXATTRS_UNSPECIFIED constant), so that we can
		 * distinguish "all attributes deliberately clear" from
		 * "didn't specify" if necessary.
		 */
		unsigned int unspecified:1;
		/* ARM/AMBA: TrustZone Secure access
		 * x86: System Management Mode access
		 */
		unsigned int secure:1;
		/* Memory access is usermode (unprivileged) */
		unsigned int user:1;
		/* Requester ID (for MSI for example) */
		unsigned int requester_id:16;
		/* Invert endianness for this page */
		unsigned int byte_swap:1;
		/*  
		 * The following are target-specific page-table bits.  These are not
		 * related to actual memory transactions at all.  However, this structure
		 * is part of the tlb_fill interface, cached in the cputlb structure,
		 * and has unused bits.  These fields will be read by target-specific
		 * helpers using env->iotlb[mmu_idx][tlb_index()].attrs.target_tlb_bitN.
		 */
		unsigned int target_tlb_bit0 : 1;
		unsigned int target_tlb_bit1 : 1;
		unsigned int target_tlb_bit2 : 1;
	} MemTxAttrs;

CPUState *qemu_get_cpu(int index)
{
    CPUState *cpu;

    CPU_FOREACH(cpu) {
        if (cpu->cpu_index == index) {
            return cpu;
        }
    }

    return NULL;
}

exec.c:
2987 MemoryRegion *get_system_memory(void)
2988 {
2989     return system_memory;
2990 }
2991 
2992 MemoryRegion *get_system_io(void)
2993 {
2994     return system_io;
2995 }

不清楚通过mr访问和as访问的具体路径和区别

#endif

