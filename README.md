# aarch64-os

a simple OS for aarch64(qemu virt board)

![sample](screenshot/shot.png)

## Features
- multiprocessor
- preemptive multi tasking
- process scheduler
  - round-robin
- GICv3 interrupt controller
- block device
  - virtio-blk
- file system
  - ext2
- UNIX-like syscalls
- isolation of address space by MMU
  - multiple address space
- command-line interface(sh)
- some core utils
  - ls, cat, echo etc...

## Required
- aarch64-linux-gnu-toolchain
- qemu-system-aarch64(>= 5.0.0)

## Build & Run

```
$ make qemu
```
specify the number of cores (default ncore=4):
```
$ make qemu NCPU=<ncore>
```

## License

MIT License
