#include "hw/sysbus.h"
#include "qemu/osdep.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "qemu/log.h"
#include "qemu/module.h"

#define WRAPPER(obj) OBJECT_CHECK(WrapperState, (obj), "wrapper")

#define LENGTH (0xFFFFFFFF)

typedef struct
{
    /* <private> */
    SysBusDevice parent_obj;
    /* <public> */
    MemoryRegion mmio;
    //qemu_irq irq;
} WrapperState;

//extern uint64_t (*wrapper_read )(void* opaque, uint64_t addr, uint32_t size);
//extern void     (*wrapper_write)(void* opaque, uint64_t addr, uint64_t val64, uint32_t size);

static uint64_t _wrapper_read(void *opaque, hwaddr addr, unsigned int size)
{
	printf("_wrapper_read\n");
	return 0;
	//return wrapper_read(opaque,addr,size);
}
static void _wrapper_write(void *opaque, hwaddr addr, uint64_t val64, unsigned int size)
{
	printf("_wrapper_write\n");
	//return wrapper_write(opaque,addr,val64,size);
}

static void wrapper_init(Object *obj)
{
	WrapperState *s = WRAPPER(obj);

	//sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

	static const MemoryRegionOps wrapper_ops = 
	{
		.read = _wrapper_read,
		.write = _wrapper_write,
		.endianness = DEVICE_NATIVE_ENDIAN,
	};
	memory_region_init_io(&s->mmio, obj, &wrapper_ops, s, "wrapper", LENGTH);

	sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static const TypeInfo wrapper_info =
{
    .name          = "wrapper",
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(WrapperState),
    .instance_init = wrapper_init,
};
static void wrapper_register_types(void)
{
	type_register_static(&wrapper_info);
}
type_init(wrapper_register_types)

