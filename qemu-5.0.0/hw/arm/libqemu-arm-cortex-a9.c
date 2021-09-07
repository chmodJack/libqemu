#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "cpu.h"
#include "hw/sysbus.h"
#include "hw/arm/boot.h"
#include "exec/address-spaces.h"
#include "sysemu/sysemu.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "qemu/error-report.h"
#include "hw/cpu/a9mpcore.h"

//#include"wrapper.h"

static qemu_irq irq_array[4];

extern void** libqemu_signal_array;

#define MPCORE_PERIPHBASE 0xF8F00000
static void init_cpu_and_mpcore_priv(MachineState* machine)
{
	/* Create the actual CPUs */
	for (int i = 0; i < machine->smp.cpus; i++)
	{
		Object* cpu = object_new(machine->cpu_type);

		if (object_property_find(cpu, "has_el2", NULL))
			object_property_set_bool(cpu, false, "has_el2", &error_fatal);

		if (object_property_find(cpu, "has_el3", NULL))
			object_property_set_bool(cpu, false, "has_el3", &error_fatal);

		if (object_property_find(cpu, "reset-cbar", NULL))
			object_property_set_int(cpu, MPCORE_PERIPHBASE, "reset-cbar", &error_fatal);

		if (object_property_find(cpu, "realized", NULL))
			object_property_set_bool(cpu, true, "realized", &error_fatal);
	}

	/* Create the private peripheral devices (including the GIC);
	 * this must happen after the CPUs are created because a15mpcore_priv
	 * wires itself up to the CPU's generic_timer gpio out lines.
	 */
	DeviceState* mpcore_priv = qdev_create(NULL, TYPE_A9MPCORE_PRIV);
	object_property_set_int(OBJECT(mpcore_priv), machine->smp.cpus, "num-cpu", &error_fatal);
	object_property_set_int(OBJECT(mpcore_priv), GIC_INTERNAL + 128, "num-irq", &error_fatal);
	qdev_init_nofail(mpcore_priv);
	sysbus_mmio_map(SYS_BUS_DEVICE(mpcore_priv), 0, MPCORE_PERIPHBASE);

	DeviceState* cpu = DEVICE(qemu_get_cpu(0));
	/* Connect the CPUs to the GIC */
	for(int i = 0; i < machine->smp.cpus; i++)
	{
		DeviceState* cpu = DEVICE(qemu_get_cpu(i));

		sysbus_connect_irq(SYS_BUS_DEVICE(mpcore_priv), i + 0 * machine->smp.cpus, qdev_get_gpio_in(cpu, ARM_CPU_IRQ ));
		sysbus_connect_irq(SYS_BUS_DEVICE(mpcore_priv), i + 1 * machine->smp.cpus, qdev_get_gpio_in(cpu, ARM_CPU_FIQ ));
		sysbus_connect_irq(SYS_BUS_DEVICE(mpcore_priv), i + 2 * machine->smp.cpus, qdev_get_gpio_in(cpu, ARM_CPU_VIRQ));
		sysbus_connect_irq(SYS_BUS_DEVICE(mpcore_priv), i + 3 * machine->smp.cpus, qdev_get_gpio_in(cpu, ARM_CPU_VFIQ));
	}
}

static void init_cpu(MachineState* machine)
{
	Object* cpu = object_new(machine->cpu_type);

	if (object_property_find(cpu, "has_el2", NULL))
		object_property_set_bool(cpu, false, "has_el2", &error_fatal);

	if (object_property_find(cpu, "has_el3", NULL))
		object_property_set_bool(cpu, false, "has_el3", &error_fatal);

	if (object_property_find(cpu, "reset-cbar", NULL))
		object_property_set_int(cpu, MPCORE_PERIPHBASE, "reset-cbar", &error_fatal);

	if (object_property_find(cpu, "realized", NULL))
		object_property_set_bool(cpu, true, "realized", &error_fatal);
}

static void init(MachineState* machine)
{
	init_cpu(machine);

	MemoryRegion* system_memory = get_system_memory();
	memory_region_add_subregion(system_memory, 0, machine->ram);

	DeviceState* cpu = DEVICE(qemu_get_cpu(0));

	irq_array[0] = qdev_get_gpio_in(cpu,ARM_CPU_IRQ);
	irq_array[1] = qdev_get_gpio_in(cpu,ARM_CPU_FIQ);
	irq_array[2] = qdev_get_gpio_in(cpu,ARM_CPU_VIRQ);
	irq_array[3] = qdev_get_gpio_in(cpu,ARM_CPU_VFIQ);

	libqemu_signal_array = irq_array;

#if 0
	DeviceState* dev = qdev_create(NULL, "wrapper");
	qdev_init_nofail(dev);
	sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, 0xFFFF0000);
#endif

	static struct arm_boot_info boot_info = {};
	boot_info.ram_size = machine->ram_size;
	boot_info.nb_cpus = machine->smp.cpus;
	boot_info.loader_start = 0;
	//boot_info.board_id = 0xd32;
	//boot_info.board_setup_addr = BOARD_SETUP_ADDR;
	//boot_info.write_board_setup = zynq_write_board_setup;
	//boot_info.smp_loader_start = map[VE_SRAM];
	//boot_info.smp_bootreg_addr = map[VE_SYSREGS] + 0x30;
	//boot_info.gic_cpu_if_addr = daughterboard->gic_cpu_if_addr;
	//boot_info.modify_dtb = vexpress_modify_dtb;
	///* When booting Linux we should be in secure state if the CPU has one. */
	//boot_info.secure_boot = vms->secure;

	arm_load_kernel(ARM_CPU(first_cpu), machine, &boot_info);
}

static void machine_init(MachineClass *mc)
{
	mc->desc = "---------------------------------------- Libqemu for ARM's Cortex-A9";
	mc->init = init;
	mc->max_cpus = 4;
	mc->ignore_memory_transaction_failures = true;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-a9");
	mc->default_ram_id = "qemu_system_ram_id";
}

DEFINE_MACHINE("libqemu-arm-cortex-a9", machine_init)

