#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/qdev-properties.h"
#include "qemu/error-report.h"
#include "hw/arm/libqemu-arm-cortex-m4-soc.h"
#include "hw/arm/boot.h"

static void init(MachineState *machine)
{
	DeviceState *dev;

	dev = qdev_create(NULL, TYPE_LIBQEMU_ARM_CORTEX_M4_SOC);
	qdev_prop_set_string(dev, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m4"));
	object_property_set_bool(OBJECT(dev), true, "realized", &error_fatal);

	armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, FLASH_SIZE);
}

static void machine_init(MachineClass *mc)
{
	mc->desc = "---------------------------------------- Libqemu for ARM's Cortex-M4";
    mc->init = init;
	mc->ignore_memory_transaction_failures = true;
}

DEFINE_MACHINE("libqemu-arm-cortex-m4", machine_init)

