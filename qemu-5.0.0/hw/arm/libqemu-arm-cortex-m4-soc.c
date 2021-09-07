#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "hw/arm/boot.h"
#include "exec/address-spaces.h"
#include "hw/arm/libqemu-arm-cortex-m4-soc.h"
#include "hw/qdev-properties.h"
#include "sysemu/sysemu.h"

extern void** libqemu_signal_array;

static qemu_irq irq_array[96];

static void initfn(Object *obj)
{
    LibqemuArmCortexM4SocState *s = LIBQEMU_ARM_CORTEX_M4_SOC(obj);

    sysbus_init_child_obj(obj, "armv7m", &s->armv7m, sizeof(s->armv7m), TYPE_ARMV7M);

#if 0
    sysbus_init_child_obj(obj, "syscfg", &s->syscfg, sizeof(s->syscfg), TYPE_STM32F2XX_SYSCFG);

    int i;

    for (i = 0; i < STM_NUM_USARTS; i++)
        sysbus_init_child_obj(obj, "usart[*]", &s->usart[i], sizeof(s->usart[i]), TYPE_STM32F2XX_USART);

    for (i = 0; i < STM_NUM_TIMERS; i++)
        sysbus_init_child_obj(obj, "timer[*]", &s->timer[i], sizeof(s->timer[i]), TYPE_STM32F2XX_TIMER);

    s->adc_irqs = OR_IRQ(object_new(TYPE_OR_IRQ));

    for (i = 0; i < STM_NUM_ADCS; i++)
        sysbus_init_child_obj(obj, "adc[*]", &s->adc[i], sizeof(s->adc[i]), TYPE_STM32F2XX_ADC);

    for (i = 0; i < STM_NUM_SPIS; i++)
        sysbus_init_child_obj(obj, "spi[*]", &s->spi[i], sizeof(s->spi[i]), TYPE_STM32F2XX_SPI);
#endif
}

static void realize(DeviceState *dev_soc, Error **errp)
{
	LibqemuArmCortexM4SocState *s = LIBQEMU_ARM_CORTEX_M4_SOC(dev_soc);

	MemoryRegion *system_memory = get_system_memory();

	//alloc
	MemoryRegion *flash = g_new(MemoryRegion, 1);
	MemoryRegion *sram = g_new(MemoryRegion, 1);
	MemoryRegion *flash_alias = g_new(MemoryRegion, 1);


	//init
	memory_region_init_rom(flash, OBJECT(dev_soc), "LIBQEMU_ARM_CORTEX_M4_SOC.flash", FLASH_SIZE, &error_fatal);
	memory_region_init_ram(sram, NULL, "LIBQEMU_ARM_CORTEX_M4_SOC.sram", SRAM_SIZE, &error_fatal);
	memory_region_init_alias(flash_alias, OBJECT(dev_soc), "LIBQEMU_ARM_CORTEX_M4_SOC.flash.alias", flash, 0, FLASH_SIZE);

	//address map
	memory_region_add_subregion(system_memory, FLASH_BASE_ADDRESS, flash);
	memory_region_add_subregion(system_memory, SRAM_BASE_ADDRESS, sram);
	memory_region_add_subregion(system_memory, 0, flash_alias);


	//init core
	DeviceState* armv7m = DEVICE(&s->armv7m);
	qdev_prop_set_uint32(armv7m, "num-irq", 96);
	qdev_prop_set_string(armv7m, "cpu-type", s->cpu_type);
	qdev_prop_set_bit(armv7m, "enable-bitband", true);

	//connect core and memory system
	object_property_set_link(OBJECT(&s->armv7m), OBJECT(get_system_memory()), "memory", &error_abort);

	//check core init state
	Error *err = NULL;
	object_property_set_bool(OBJECT(&s->armv7m), true, "realized", &err);
	if (err != NULL)
	{
		error_propagate(errp, err);
		return;
	}

	//connect armv7 nvic irq
	for(int i=0; i<96; i++)
		irq_array[i] = qdev_get_gpio_in(armv7m,i);

	libqemu_signal_array = irq_array;

#if 0
	/* System configuration controller */
	DeviceState* dev = DEVICE(&s->syscfg);
	object_property_set_bool(OBJECT(&s->syscfg), true, "realized", &err);
	if (err != NULL)
	{
		error_propagate(errp, err);
		return;
	}
	SysBusDevice* busdev = SYS_BUS_DEVICE(dev);
	sysbus_mmio_map(busdev, 0, 0x40013800);
	sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, 71));

	int i;

	/* Attach UART (uses USART registers) and USART controllers */
	for (i = 0; i < STM_NUM_USARTS; i++) {
		dev = DEVICE(&(s->usart[i]));
		qdev_prop_set_chr(dev, "chardev", serial_hd(i));
		object_property_set_bool(OBJECT(&s->usart[i]), true, "realized", &err);
		if (err != NULL) {
			error_propagate(errp, err);
			return;
		}
		busdev = SYS_BUS_DEVICE(dev);
		sysbus_mmio_map(busdev, 0, usart_addr[i]);
		sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, usart_irq[i]));
	}

	/* Timer 2 to 5 */
	for (i = 0; i < STM_NUM_TIMERS; i++) {
		dev = DEVICE(&(s->timer[i]));
		qdev_prop_set_uint64(dev, "clock-frequency", 1000000000);
		object_property_set_bool(OBJECT(&s->timer[i]), true, "realized", &err);
		if (err != NULL) {
			error_propagate(errp, err);
			return;
		}
		busdev = SYS_BUS_DEVICE(dev);
		sysbus_mmio_map(busdev, 0, timer_addr[i]);
		sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, timer_irq[i]));
	}

	/* ADC 1 to 3 */
	object_property_set_int(OBJECT(s->adc_irqs), STM_NUM_ADCS,
			"num-lines", &err);
	object_property_set_bool(OBJECT(s->adc_irqs), true, "realized", &err);
	if (err != NULL) {
		error_propagate(errp, err);
		return;
	}
	qdev_connect_gpio_out(DEVICE(s->adc_irqs), 0,
			qdev_get_gpio_in(armv7m, ADC_IRQ));

	for (i = 0; i < STM_NUM_ADCS; i++) {
		dev = DEVICE(&(s->adc[i]));
		object_property_set_bool(OBJECT(&s->adc[i]), true, "realized", &err);
		if (err != NULL) {
			error_propagate(errp, err);
			return;
		}
		busdev = SYS_BUS_DEVICE(dev);
		sysbus_mmio_map(busdev, 0, adc_addr[i]);
		sysbus_connect_irq(busdev, 0,
				qdev_get_gpio_in(DEVICE(s->adc_irqs), i));
	}

	/* SPI 1 and 2 */
	for (i = 0; i < STM_NUM_SPIS; i++) {
		dev = DEVICE(&(s->spi[i]));
		object_property_set_bool(OBJECT(&s->spi[i]), true, "realized", &err);
		if (err != NULL) {
			error_propagate(errp, err);
			return;
		}
		busdev = SYS_BUS_DEVICE(dev);
		sysbus_mmio_map(busdev, 0, spi_addr[i]);
		sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, spi_irq[i]));
	}

#endif
}

static Property properties[] =
{
    DEFINE_PROP_STRING("cpu-type", LibqemuArmCortexM4SocState, cpu_type),
    DEFINE_PROP_END_OF_LIST(),
};

static void class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = realize;
    device_class_set_props(dc, properties);
}

static const TypeInfo libqemu_arm_cortex_m4_soc_info =
{
    .name          = TYPE_LIBQEMU_ARM_CORTEX_M4_SOC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(LibqemuArmCortexM4SocState),
    .instance_init = initfn,
    .class_init    = class_init,
};

static void libqemu_arm_cortex_m4_soc_types(void)
{
    type_register_static(&libqemu_arm_cortex_m4_soc_info);
}

type_init(libqemu_arm_cortex_m4_soc_types)

