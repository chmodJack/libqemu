#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/error-report.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "hw/sysbus.h"

#include "target/riscv/cpu.h"
#include "hw/riscv/riscv_hart.h"

#include "hw/riscv/sifive_plic.h"
#include "hw/riscv/sifive_clint.h"





#include "hw/riscv/riscv_hart.h"
#include "hw/riscv/sifive_cpu.h"

#define TYPE_LIBQEMU_RISCV32_SOC "libqemu.riscv32.soc"
#define LIBQEMU_RISCV32_SOC(obj) OBJECT_CHECK(LibqemuRiscv32SoCState, (obj), TYPE_LIBQEMU_RISCV32_SOC)

typedef struct LibqemuRiscv32SoCState
{
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    RISCVHartArrayState cpus;
    DeviceState *plic;
} LibqemuRiscv32SoCState;

typedef struct LibqemuRiscv32State
{
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    LibqemuRiscv32SoCState soc;
} LibqemuRiscv32State;


#include "hw/riscv/sifive_e_prci.h"

#include "hw/riscv/boot.h"
#include "chardev/char.h"
#include "sysemu/arch_init.h"
#include "sysemu/sysemu.h"
#include "exec/address-spaces.h"
 


static void libqemu_riscv32_soc_init(Object *obj)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    LibqemuRiscv32SoCState *s = LIBQEMU_RISCV32_SOC(obj);

    object_initialize_child(obj, "cpus", &s->cpus, sizeof(s->cpus), TYPE_RISCV_HART_ARRAY, &error_abort, NULL);

    object_property_set_str(OBJECT(&s->cpus), SIFIVE_E_CPU, "cpu-type", &error_abort);
    object_property_set_int(OBJECT(&s->cpus), ms->smp.cpus, "num-harts", &error_abort);
}

#if 0
static void libqemu_riscv32_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    const struct MemmapEntry *memmap = sifive_e_memmap;
    Error *err = NULL;

    LibqemuRiscv32SoCState *s = LIBQEMU_RISCV32_SOC(dev);
    MemoryRegion *sys_mem = get_system_memory();

    object_property_set_bool(OBJECT(&s->cpus), true, "realized", &error_abort);

    /* Mask ROM */
    memory_region_init_rom(&s->mask_rom, OBJECT(dev), "libqemu.riscv32.mrom", memmap[SIFIVE_E_MROM].size, &error_fatal);
    memory_region_add_subregion(sys_mem, memmap[SIFIVE_E_MROM].base, &s->mask_rom);

    /* MMIO */
    s->plic = sifive_plic_create(memmap[SIFIVE_E_PLIC].base,
        (char *)SIFIVE_E_PLIC_HART_CONFIG,
        SIFIVE_E_PLIC_NUM_SOURCES,
        SIFIVE_E_PLIC_NUM_PRIORITIES,
        SIFIVE_E_PLIC_PRIORITY_BASE,
        SIFIVE_E_PLIC_PENDING_BASE,
        SIFIVE_E_PLIC_ENABLE_BASE,
        SIFIVE_E_PLIC_ENABLE_STRIDE,
        SIFIVE_E_PLIC_CONTEXT_BASE,
        SIFIVE_E_PLIC_CONTEXT_STRIDE,
        memmap[SIFIVE_E_PLIC].size);
    sifive_clint_create(memmap[SIFIVE_E_CLINT].base, memmap[SIFIVE_E_CLINT].size, ms->smp.cpus, SIFIVE_SIP_BASE, SIFIVE_TIMECMP_BASE, SIFIVE_TIME_BASE, false);
    create_unimplemented_device("libqemu.riscv32.aon", memmap[SIFIVE_E_AON].base, memmap[SIFIVE_E_AON].size);
	sifive_e_prci_create(memmap[SIFIVE_E_PRCI].base);

    /* GPIO */
    object_property_set_bool(OBJECT(&s->gpio), true, "realized", &err);
    if (err)
	{
        error_propagate(errp, err);
        return;
    }

    /* Map GPIO registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gpio), 0, memmap[SIFIVE_E_GPIO0].base);

    /* Pass all GPIOs to the SOC layer so they are available to the board */
    qdev_pass_gpios(DEVICE(&s->gpio), dev, NULL);

    /* Connect GPIO interrupts to the PLIC */
    for (int i = 0; i < 32; i++)
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gpio), i, qdev_get_gpio_in(DEVICE(s->plic), SIFIVE_E_GPIO0_IRQ0 + i));

    sifive_uart_create(sys_mem, memmap[SIFIVE_E_UART0].base, serial_hd(0), qdev_get_gpio_in(DEVICE(s->plic), SIFIVE_E_UART0_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi0", memmap[SIFIVE_E_QSPI0].base, memmap[SIFIVE_E_QSPI0].size);
    create_unimplemented_device("riscv.sifive.e.pwm0", memmap[SIFIVE_E_PWM0].base, memmap[SIFIVE_E_PWM0].size);
    sifive_uart_create(sys_mem, memmap[SIFIVE_E_UART1].base, serial_hd(1), qdev_get_gpio_in(DEVICE(s->plic), SIFIVE_E_UART1_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi1", memmap[SIFIVE_E_QSPI1].base, memmap[SIFIVE_E_QSPI1].size);
    create_unimplemented_device("riscv.sifive.e.pwm1", memmap[SIFIVE_E_PWM1].base, memmap[SIFIVE_E_PWM1].size);
    create_unimplemented_device("riscv.sifive.e.qspi2", memmap[SIFIVE_E_QSPI2].base, memmap[SIFIVE_E_QSPI2].size);
    create_unimplemented_device("riscv.sifive.e.pwm2", memmap[SIFIVE_E_PWM2].base, memmap[SIFIVE_E_PWM2].size);

    /* Flash memory */
    memory_region_init_rom(&s->xip_mem, OBJECT(dev), "riscv.sifive.e.xip", memmap[SIFIVE_E_XIP].size, &error_fatal);
    memory_region_add_subregion(sys_mem, memmap[SIFIVE_E_XIP].base, &s->xip_mem);
}

#endif

static void libqemu_riscv32_soc_realize(DeviceState* dev, Error** errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());

    LibqemuRiscv32SoCState *s = LIBQEMU_RISCV32_SOC(dev);
    MemoryRegion *sys_mem = get_system_memory();

    object_property_set_bool(OBJECT(&s->cpus), true, "realized", &error_abort);

	MemoryRegion* ram = g_new(MemoryRegion, 1);
	memory_region_init_ram(ram, NULL, "LIBQEMU_RISCV32.ram", 1024 * 64, &error_fatal);
	memory_region_add_subregion(sys_mem, 0, ram);

	//s->plic = sifive_plic_create(memmap[SIFIVE_E_PLIC].base, (char *)SIFIVE_E_PLIC_HART_CONFIG, SIFIVE_E_PLIC_NUM_SOURCES, SIFIVE_E_PLIC_NUM_PRIORITIES, SIFIVE_E_PLIC_PRIORITY_BASE, SIFIVE_E_PLIC_PENDING_BASE, SIFIVE_E_PLIC_ENABLE_BASE, SIFIVE_E_PLIC_ENABLE_STRIDE, SIFIVE_E_PLIC_CONTEXT_BASE, SIFIVE_E_PLIC_CONTEXT_STRIDE, memmap[SIFIVE_E_PLIC].size);
	s->plic = sifive_plic_create(0xC000000, (char*)"M", 127, 7, 0x04, 0x1000, 0x2000, 0x80, 0x200000, 0x1000, 0x4000000);
    sifive_clint_create(0x2000000, 0x10000, ms->smp.cpus, SIFIVE_SIP_BASE, SIFIVE_TIMECMP_BASE, SIFIVE_TIME_BASE, false);
	sifive_e_prci_create(0xC000000);

#if 0
    Error *err = NULL;

    /* GPIO */
    object_property_set_bool(OBJECT(&s->gpio), true, "realized", &err);
    if (err) { error_propagate(errp, err); return; }

    /* Map GPIO registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gpio), 0, memmap[SIFIVE_E_GPIO0].base);

    /* Pass all GPIOs to the SOC layer so they are available to the board */
    qdev_pass_gpios(DEVICE(&s->gpio), dev, NULL);

    /* Connect GPIO interrupts to the PLIC */
    for (int i = 0; i < 32; i++)
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gpio), i, qdev_get_gpio_in(DEVICE(s->plic), SIFIVE_E_GPIO0_IRQ0 + i));

    sifive_uart_create(sys_mem, memmap[SIFIVE_E_UART0].base, serial_hd(0), qdev_get_gpio_in(DEVICE(s->plic), SIFIVE_E_UART0_IRQ));
    sifive_uart_create(sys_mem, memmap[SIFIVE_E_UART1].base, serial_hd(1), qdev_get_gpio_in(DEVICE(s->plic), SIFIVE_E_UART1_IRQ));

#endif

}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




static void libqemu_riscv32_soc_class_init(ObjectClass* oc, void* data)
{
    DeviceClass* dc = DEVICE_CLASS(oc);

    dc->realize = libqemu_riscv32_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}
static const TypeInfo libqemu_riscv32_soc_type_info =
{
    .name = TYPE_LIBQEMU_RISCV32_SOC,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(LibqemuRiscv32SoCState),
    .instance_init = libqemu_riscv32_soc_init,
    .class_init = libqemu_riscv32_soc_class_init,
};
static void libqemu_riscv32_soc_register_types(void)
{
    type_register_static(&libqemu_riscv32_soc_type_info);
}
type_init(libqemu_riscv32_soc_register_types)


static void libqemu_riscv32_init(MachineState* machine)
{
	LibqemuRiscv32State* s = g_new0(LibqemuRiscv32State, 1);

	object_initialize_child(OBJECT(machine), "soc", &s->soc, sizeof(s->soc), TYPE_LIBQEMU_RISCV32_SOC, &error_abort, NULL);
	object_property_set_bool(OBJECT(&s->soc), true, "realized", &error_abort);

	if (machine->kernel_filename)
		riscv_load_kernel(machine->kernel_filename, NULL);
}
static void libqemu_riscv32_machine_init(MachineClass* mc)
{
	mc->desc = "---------------------------------------- Libqemu for RISCV32";
    mc->init = libqemu_riscv32_init;
    mc->max_cpus = 1;
}
DEFINE_MACHINE("libqemu-riscv32", libqemu_riscv32_machine_init)

