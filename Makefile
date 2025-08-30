
SRCS = $(shell dir /s /b *.c)
OBJS = $(SRCS:.c=.o)
INCS = $(shell dir /s /b *include)
CFLAGS = -fpic -ffreestanding

TARGET_PATH = .\target
INCLUDE_CONFIG = ${TARGET_PATH}\include\config
BUILD_PATH = ${TARGET_PATH}\build
OUT_OBJS = ${BUILD_PATH}\obj
OUT_ELF = ${BUILD_PATH}\elf
OUT_IMG = ${BUILD_PATH}\img

all: clean $(TARGET_DEVICE) compile-boot compile-kernel link-kernel

arm-raspi2:
	@echo ${SRCS}
	@echo Setting up configuration for device ${TARGET_DEVICE}
	if not exist "${INCLUDE_CONFIG}" mkdir ${INCLUDE_CONFIG}
	copy /Y ".\config\raspi2_config.h" "${INCLUDE_CONFIG}\config.h"
	$(eval ARCH_BOOTPATH = .\boot\arm\mach-bcm\bcm2836)

arm-raspi3: 
	@echo Setting up configuration for device ${TARGET_DEVICE}
	if not exist "${INCLUDE_CONFIG}" mkdir ${INCLUDE_CONFIG}
	copy /Y ".\config\raspi3_config.h" "${INCLUDE_CONFIG}\config.h"
	$(eval ARCH_BOOTPATH = .\boot\arm\mach-bcm\bcm2837)

compile-boot:
	@echo boot kääntö ${ARCH_BOOTPATH}
	if not exist "${OUT_OBJS}" mkdir ${OUT_OBJS}
	arm-none-eabi-gcc -mcpu=cortex-a7 ${CFLAGS} -c ${ARCH_BOOTPATH}\boot.s -o ${OUT_OBJS}\boot.o

compile-kernel: $(OBJS)
	@echo kernel kääntö
	if not exist "${OUT_OBJS}" mkdir ${OUT_OBJS}
	
link-kernel:
	if not exist "${OUT_ELF}" mkdir ${OUT_ELF}
	if not exist "${OUT_IMG}" mkdir ${OUT_IMG}
	arm-none-eabi-gcc -T ${ARCH_BOOTPATH}\linker.ld -o ${OUT_ELF}\evolution.elf -ffreestanding -O2 -nostdlib ${OUT_OBJS}\boot.o $(addprefix ${OUT_OBJS}\, $(notdir $(OBJS))) -lgcc
	arm-none-eabi-objcopy ${OUT_ELF}\evolution.elf -O binary ${OUT_IMG}\evolution.img

%.o: %.c
	arm-none-eabi-gcc $(addprefix -I, ${INCS}) -mcpu=arm1176jzf-s -std=gnu99 ${CFLAGS} -c $< -o ${OUT_OBJS}\$(notdir $@) -O2 -Wall -Wextra

clean:
	@echo Cleaning up generated files...
	if exist "${TARGET_PATH}" rmdir /S /Q ${TARGET_PATH}
	if exist "${TARGET_PATH}\config.h" del "${TARGET_PATH}\config.h"