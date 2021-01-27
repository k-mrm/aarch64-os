PREFIX = aarch64-linux-gnu-
GCC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

QEMU = qemu-system-aarch64
CPU = cortex-a72
MACHINE = raspi3

qemu:
	$(QEMU) -cpu $(CPU) -machine $(MACHINE)
