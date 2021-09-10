PREFIX = aarch64-linux-gnu-
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

CPU = cortex-a72

CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -DUSE_ARMVIRT -mcpu=$(CPU)
CFLAGS += -I ./include/
CFLAGS += -I ./include/lib/
LDFLAGS = -nostdlib -nostartfiles

QEMU = qemu-system-aarch64
MACHINE = virt
MACHINE_GIC = gic-version=2
NCPU = 1

KOBJS = kernel/boot.o kernel/vectortable.o	\
			 kernel/console.o kernel/trap.o kernel/font.o \
			 kernel/main.o kernel/printk.o kernel/proc.o kernel/kalloc.o	\
			 kernel/cswitch.o kernel/syscall.o kernel/systable.o kernel/mm.o

DRIVER = kernel/driver/gicv2.o kernel/driver/timer.o

VIRTDRV = $(DRIVER) kernel/driver/virt/uart.o

RPI4DRV = $(DRIVER)

LIBS = lib/string.o

OBJS = $(KOBJS)	$(LIBS) $(VIRTDRV)

SDPATH = /media/k-mrm/09D0-F0A8
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.elf: $(OBJS) kernel/kernel.ld
	$(LD) $(LDFLAGS) $(OBJS) -T kernel/kernel.ld -o kernel8.elf

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

qemu: kernel8.img
	$(QEMU) -cpu $(CPU) -machine $(MACHINE),$(MACHINE_GIC) -d mmu -D ./qemu.log -smp $(NCPU) -m 128 -nographic -kernel kernel8.elf

gdb: kernel8.img
	$(QEMU) -S -gdb tcp::1234 -cpu $(CPU) -machine $(MACHINE),$(MACHINE_GIC) -smp $(NCPU) -m 128 -nographic -kernel kernel8.elf

dts:
	$(QEMU) -S -cpu $(CPU) -machine $(MACHINE),$(MACHINE_GIC),dumpdtb=virt.dtb -smp $(NCPU) -nographic
	dtc -I dtb -O dts -o virt.dts virt.dtb
	rm virt.dtb

raspi: kernel8.img
	cp kernel8.img $(SDPATH)

clean:
	$(RM) $(OBJS) kernel8.elf kernel8.img

.PHONY: qemu gdb clean dts raspi
