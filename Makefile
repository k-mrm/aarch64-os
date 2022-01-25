PREFIX = aarch64-linux-gnu-
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

CPU = cortex-a72+nofp

CFLAGS = -Wall -Og -g -MD -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=$(CPU)
#CFLAGS += -DKDBG
CFLAGS += -DUSE_ARMVIRT
CFLAGS += -I ./include/
LDFLAGS = -nostdlib -nostartfiles

QEMUPREFIX =
QEMU = $(QEMUPREFIX)qemu-system-aarch64
MACHINE = virt
MACHINE_GIC = gic-version=2
ifndef NCPU
NCPU = 4
endif

QCPU = cortex-a72
QEMUOPTS = -cpu $(QCPU) -machine $(MACHINE),$(MACHINE_GIC) -smp $(NCPU) -m 256
QEMUOPTS += -global virtio-mmio.force-legacy=false
QEMUOPTS += -drive file=fs.img,if=none,format=raw,id=d0
QEMUOPTS += -device virtio-blk-device,drive=d0,bus=virtio-mmio-bus.0
QEMUOPTS += -nographic -kernel kernel8.elf

KOBJS = kernel/boot.o kernel/vectortable.o kernel/file.o kernel/buf.o \
				kernel/console.o kernel/trap.o kernel/font.o kernel/ext2.o kernel/uname.o \
				kernel/main.o kernel/printk.o kernel/proc.o kernel/kalloc.o kernel/fs.o	\
				kernel/cswitch.o kernel/syscall.o kernel/mm.o kernel/string.o kernel/elf.o	\
				kernel/cdev.o kernel/spinlock.o

DRIVER = kernel/driver/gicv2.o kernel/driver/timer.o kernel/driver/virtio.o	\
				 kernel/driver/psci-if.o kernel/driver/psci.o

VIRTDRV = $(DRIVER) kernel/driver/virt/uart.o

RPI4DRV = $(DRIVER)

OBJS = $(KOBJS)	$(VIRTDRV)

SDPATH = /media/k-mrm/09D0-F0A8

usr/initcode: usr/initcode.S
	$(CC) $(CFLAGS) -c usr/initcode.S -o usr/initcode.o
	$(LD) $(LDFLAGS) -N -e initcode -Ttext 0x1000 -o usr/initcode.elf usr/initcode.o
	$(OBJCOPY) -S -O binary usr/initcode.elf usr/initcode

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

-include: *.d

ULIBS = usr/systable.o usr/ulib.o usr/malloc.o

root = rootfs

UPROGS = $(root)/init $(root)/sh $(root)/cat $(root)/echo $(root)/ls $(root)/uname	\
				 $(root)/mkdir $(root)/loop $(root)/writetest	$(root)/thread $(root)/heap 	\
				 #$(root)/malloctest

$(root)/%: usr/%.o $(ULIBS)
	@mkdir -p $(root)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0x1000 -o $@ $^
	cp README.md $(root)/

fs.img: $(UPROGS)
	dd if=/dev/zero of=fs.img count=10000
	mkfs -t ext2 -d rootfs/ -v fs.img -b 1024

kernel8.elf: $(OBJS) kernel/kernel.ld usr/initcode
	$(LD) -r -b binary usr/initcode -o usr/initcode.o
	$(LD) $(LDFLAGS) -T kernel/kernel.ld -o $@ $(OBJS) usr/initcode.o

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary $^ $@

qemu: kernel8.img fs.img
	$(QEMU) $(QEMUOPTS)

gdb: kernel8.img fs.img
	@echo "*** aarch64-linux-gnu-gdb -x connect.gdb"
	$(QEMU) -S -gdb tcp::1234 $(QEMUOPTS)

dts:
	$(QEMU) -S -cpu $(QCPU) -machine $(MACHINE),$(MACHINE_GIC),dumpdtb=virt.dtb -smp $(NCPU) -nographic
	dtc -I dtb -O dts -o virt.dts virt.dtb
	$(RM) virt.dtb

raspi: kernel8.img fs.img
	cp kernel8.img $(SDPATH)

clean:
	$(RM) $(OBJS) $(ULIBS) $(UOBJS) $(UPROGS) usr/initcode.o usr/initcode.elf usr/initcode kernel8.elf kernel8.img fs.img
	find ./ -name "*.d" | xargs $(RM)
	$(RM) -rf $(root)

.PHONY: qemu gdb clean dts raspi
