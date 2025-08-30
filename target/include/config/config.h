/********************************
 Laitekohtaiset konfiguraatiot -> raspi2
*********************************/
#pragma once

#include <bcm2836/bcm2836.h>

#define TARGET_ARCH		        "Arm"
#define TARGET_CHIPSET_FAMILY   "BCM"
#define TARGET_CHIPSET	        "BCM2836"

#define MODULE_UART
#define MODULE_MMC
#define MODULE_FRAMEBUFFER
#define MODULE_DPI_TOUCHSCREEN
#define MODULE_MB