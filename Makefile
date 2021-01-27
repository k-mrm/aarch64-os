PREFIX = aarch64-linux-gnu-
GCC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

CCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
LDFLAGS = -nostdlib -nostartfiles

QEMU = qemu-system-aarch64
CPU = cortex-a72
MACHINE = raspi3

boot.o: boot.S
	$(GCC) $(CCFLAGS) -c boot.S -o boot.o

kernel8.img:
	$(LD) $(LDFLAGS) boot.o -T link.ld -o kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

qemu: kernel8.img
	$(QEMU) -cpu $(CPU) -machine $(MACHINE) -kernel kernel8.img

.PHONY: clean

clean:
	$(RM) boot.o
