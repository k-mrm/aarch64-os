#ifndef AARCH64_OS_KERNEL_H
#define AARCH64_OS_KERNEL_H

#ifndef __ASSEMBLER__

typedef unsigned long u64;
typedef long i64;
typedef unsigned int u32;
typedef signed int i32;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned char u8;
typedef signed char i8;

#define NULL ((void *)0)

typedef _Bool bool;

#define true 1
#define false 0

#define REG(addr) (*(volatile u32 *)(u64)(addr))

#define min(a, b) (((a) < (b))? (a) : (b))

#endif

#endif
