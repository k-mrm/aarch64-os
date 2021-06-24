PREFIX = aarch64-linux-gnu-
GCC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

CCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -DUSE_ARMVIRT
CCFLAGS += -I ./include/
CCFLAGS += -I ./include/lib/
LDFLAGS = -nostdlib -nostartfiles

QEMU = qemu-system-aarch64
CPU = cortex-a72
MACHINE = virt
MACHINE_GIC = gic-version=2

KOBJS = kernel/boot.o kernel/vectortable.o	\
			 kernel/console.o kernel/trap.o kernel/font.o kernel/gicv2.o \
			 kernel/main.o kernel/printk.o kernel/systimer.o	\
			 kernel/proc.o kernel/cswitch.o	kernel/forkret.o

VIRTDRV = kernel/driver/virt/uart.o

RPI4DRV = 

LIBS = lib/string.o

OBJS = $(KOBJS)	$(LIBS) $(VIRTDRV)

SDPATH = /media/k-mrm/09D0-F0A8
%.o: %.c
	$(GCC) $(CCFLAGS) -c $< -o $@

%.o: %.S
	$(GCC) $(CCFLAGS) -c $< -o $@

kernel8.img: $(OBJS) kernel/link.ld
	$(LD) $(LDFLAGS) $(OBJS) -T kernel/link.ld -o kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

qemu: kernel8.img
	$(QEMU) -cpu $(CPU) -machine $(MACHINE),$(MACHINE_GIC) -nographic -kernel kernel8.elf

raspi: kernel8.img
	cp kernel8.img $(SDPATH)

.PHONY: clean

clean:
	$(RM) $(OBJS) kernel8.elf kernel8.img
