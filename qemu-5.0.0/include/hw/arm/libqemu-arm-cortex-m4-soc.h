#ifndef __LIBQEMU_ARM_CORTEX_M4_SOC_H__
#define __LIBQEMU_ARM_CORTEX_M4_SOC_H__

#include "hw/arm/armv7m.h"

#define TYPE_LIBQEMU_ARM_CORTEX_M4_SOC "libqemu-arm-cortex-m4-soc"

#define LIBQEMU_ARM_CORTEX_M4_SOC(obj) \
    OBJECT_CHECK(LibqemuArmCortexM4SocState, (obj), TYPE_LIBQEMU_ARM_CORTEX_M4_SOC)


#define FLASH_BASE_ADDRESS 0x08000000
#define FLASH_SIZE (1024 * 1024)
#define SRAM_BASE_ADDRESS 0x20000000
#define SRAM_SIZE (128 * 1024)

typedef struct LibqemuArmCortexM4SocState
{
    /*< private >*/
    SysBusDevice parent_obj;
    /*< public >*/
    char *cpu_type;
    ARMv7MState armv7m;
}LibqemuArmCortexM4SocState;

#endif//__LIBQEMU_ARM_CORTEX_M4_SOC_H__
