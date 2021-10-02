PREFIX = aarch64-linux-gnu-
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

CPU = cortex-a72

CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=$(CPU)
CFLAGS += -DUSE_ARMVIRT
CFLAGS += -I ./include/
LDFLAGS = -nostdlib -nostartfiles

QEMU = qemu-system-aarch64
MACHINE = virt
MACHINE_GIC = gic-version=2
NCPU = 1

QEMUOPTS = -cpu $(CPU) -machine $(MACHINE),$(MACHINE_GIC) -smp $(NCPU) -m 128
#QEMUOPTS += -drive file=fs.img,if=none,format=raw,id=d0
#QEMUOPTS += -device virtio-blk-device,drive=d0
QEMUOPTS += -nographic -kernel kernel8.elf

KOBJS = kernel/boot.o kernel/vectortable.o kernel/ramdisk.o	\
			 kernel/console.o kernel/trap.o kernel/font.o kernel/ext2.o \
			 kernel/main.o kernel/printk.o kernel/proc.o kernel/kalloc.o	\
			 kernel/cswitch.o kernel/syscall.o kernel/mm.o kernel/string.o kernel/elf.o

DRIVER = kernel/driver/gicv2.o kernel/driver/timer.o

VIRTDRV = $(DRIVER) kernel/driver/virt/uart.o

RPI4DRV = $(DRIVER)

OBJS = $(KOBJS)	$(VIRTDRV)

SDPATH = /media/k-mrm/09D0-F0A8
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@


ULIBS = usr/systable.o usr/ulib.o

UOBJS = usr/test.o usr/init.o

UPROGS = usr/test usr/init

usr/%: usr/%.o $(ULIBS)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^

fs.img: $(UPROGS)
	dd if=/dev/zero of=fs.img count=10000
	mkfs -t ext2 -v fs.img -b 1024

kernel8.elf: $(OBJS) kernel/kernel.ld fs.img
	$(LD) -r -b binary fs.img -o fs.img.o
	$(LD) $(LDFLAGS) -T kernel/kernel.ld -o $@ $(OBJS) fs.img.o

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary $^ $@

qemu: kernel8.img
	$(QEMU) $(QEMUOPTS)

gdb: kernel8.img
	$(QEMU) -S -gdb tcp::1234 $(QEMUOPTS)

dts:
	$(QEMU) -S -cpu $(CPU) -machine $(MACHINE),$(MACHINE_GIC),dumpdtb=virt.dtb -smp $(NCPU) -nographic
	dtc -I dtb -O dts -o virt.dts virt.dtb
	rm virt.dtb

raspi: kernel8.img fs.img
	cp kernel8.img $(SDPATH)

clean:
	$(RM) $(OBJS) $(ULIBS) $(UOBJS) $(UPROGS) kernel8.elf kernel8.img fs.img

.PHONY: qemu gdb clean dts raspi
