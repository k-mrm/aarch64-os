PREFIX = aarch64-linux-gnu-
GCC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

CCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
LDFLAGS = -nostdlib -nostartfiles

QEMU = qemu-system-aarch64
CPU = cortex-a72
MACHINE = raspi3

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

SDPATH = /media/k-mrm/9016-4EF8

%.o: %.c
	$(GCC) $(CCFLAGS) -c $< -o $@

boot.o: boot.S
	$(GCC) $(CCFLAGS) -c boot.S -o boot.o

kernel8.img: boot.o $(OBJS)
	$(LD) $(LDFLAGS) boot.o $(OBJS) -T link.ld -o kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

qemu: kernel8.img
	$(QEMU) -cpu $(CPU) -machine $(MACHINE) -kernel kernel8.img

raspi: kernel8.img
	cp kernel8.img $(SDPATH)
	cp bin/start4.elf $(SDPATH)
	cp bin/fixup.dat $(SDPATH)

.PHONY: clean

clean:
	$(RM) boot.o $(OBJS)
