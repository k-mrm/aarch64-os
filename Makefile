PREFIX = aarch64-linux-gnu-
GCC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

CCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
LDFLAGS = -nostdlib -nostartfiles

QEMU = qemu-system-aarch64
CPU = cortex-a72
MACHINE = raspi3

OBJS = kernel/boot.o kernel/vectortable.o	\
			 kernel/console.o kernel/exception.o kernel/font.o kernel/framebuffer.o kernel/gicv2.o \
			 kernel/gpio.o kernel/mailbox.o kernel/main.o kernel/printk.o kernel/systimer.o

SDPATH = /media/k-mrm/09D0-F0A8

%.o: %.c
	$(GCC) $(CCFLAGS) -c $< -o $@

%.o: %.S
	$(GCC) $(CCFLAGS) -c $< -o $@

kernel8.img: $(OBJS) kernel/link.ld
	$(LD) $(LDFLAGS) $(OBJS) -T kernel/link.ld -o kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

qemu: kernel8.img
	$(QEMU) -cpu $(CPU) -machine $(MACHINE) -kernel kernel8.img

raspi: kernel8.img
	cp kernel8.img $(SDPATH)

.PHONY: clean

clean:
	$(RM) $(OBJS) kernel8.elf kernel8.img
