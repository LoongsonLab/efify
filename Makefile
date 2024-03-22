# SPDX-License-Identifier: GPL-2.0
#
# Author: Huacai Chen <chenhuacai@loongson.cn>
# Copyright (C) 2020 Loongson Technology Corporation Limited
# 
# Modified to be independent of linux kernel source

BOOT_HEAP_SIZE := 0x400000
KERNEL_LOAD_ADDRESS ?= 0x9000000000200000
KERNEL_ENTRY_ADDRESS ?= 0x90000000011b7000

CROSS_PREFIX := loongarch64-linux-gnu-
CC := ${CROSS_PREFIX}gcc
OBJCOPY := ${CROSS_PREFIX}objcopy
LD := ${CROSS_PREFIX}ld
STRIP := ${CROSS_PREFIX}ld

KBUILD_CFLAGS = -nostdinc -Wall -mabi=lp64s -msoft-float -fno-builtin-memcpy -fno-builtin-memmove -fno-builtin-memset -O2 -Werror -g -gdwarf-4 -I. -fpie -Wa,-mla-global-with-pcrel
KBUILD_AFLAGS = -nostdinc -Wall -mabi=lp64s -msoft-float -fno-builtin-memcpy -fno-builtin-memmove -fno-builtin-memset -O2 -Werror -g -gdwarf-4 -I. -fpie -Wa,-mla-global-with-pcrel

KBUILD_CFLAGS := $(KBUILD_CFLAGS) \
	-DBOOT_HEAP_SIZE=$(BOOT_HEAP_SIZE) \
	-D"KERNEL_LOAD_ADDRESS_ULL=$(KERNEL_LOAD_ADDRESS)ull" \
	-D"KERNEL_ENTRY=$(KERNEL_ENTRY_ADDRESS)ull" \

KBUILD_AFLAGS := $(KBUILD_AFLAGS) -D__ASSEMBLY__ \
	-DBOOT_HEAP_SIZE=$(BOOT_HEAP_SIZE) \
	-DKERNEL_ENTRY=$(KERNEL_ENTRY_ADDRESS)

all: kernel.efi

kernel.bin: kernel
	$(OBJCOPY)  -O binary -R .comment -S kernel kernel.bin

piggy.o: dummy.o kernel.bin
	$(OBJCOPY) --add-section=.image=./kernel.bin \
               --set-section-flags=.image=contents,alloc,load,readonly,data \
               dummy.o piggy.o

efiobjs := efi-header.o piggy.o \
		string.o libefistub.a

ifdef CONFIG_DEBUG
efiobjs	+= dbg.o uart-16550.o
endif

kernel.efi: ld.script $(efiobjs)
	$(LD) -m elf64loongarch -z noexecstack -T ./ld.script $(efiobjs) -o kernel.elf
	$(OBJCOPY) -O binary -R .comment -S kernel.elf kernel.efi

.c.o:
	$(CC) $(KBUILD_CFLAGS) $< -c -o $@

.S.o:
	$(CC) $(KBUILD_AFLAGS) $< -c -o $@

clean:
	rm -f *.bin *.o *.z *.efi *.elf

