#include "kernel.h"
#include "memmap.h"
#include "printk.h"
#include "trap.h"
#include "log.h"

#define GICD_BASE (GICBASE)
#define GICC_BASE (GICBASE + 0x10000)

#define GICD_CTLR           (GICD_BASE)
#define GICD_TYPER          (GICD_BASE + 0x4)
#define GICD_ISENABLER(n)   (GICD_BASE + 0x100 + (u64)(n) * 4)
#define GICD_ICENABLER(n)   (GICD_BASE + 0x180 + (u64)(n) * 4)
#define GICD_ISPENDR(n)     (GICD_BASE + 0x200 + (u64)(n) * 4)
#define GICD_ICPENDR(n)     (GICD_BASE + 0x280 + (u64)(n) * 4)
#define GICD_IPRIORITYR(n)  (GICD_BASE + 0x400 + (u64)(n) * 4)
#define GICD_ITARGETSR(n)   (GICD_BASE + 0x800 + (u64)(n) * 4)
#define GICD_ICFGR(n)       (GICD_BASE + 0xc00 + (u64)(n) * 4)

#define GICC_CTLR   (GICC_BASE)
#define GICC_PMR    (GICC_BASE + 0x4)
#define GICC_IAR    (GICC_BASE + 0xc)
#define GICC_EOIR   (GICC_BASE + 0x10)
#define GICC_HPPIR  (GICC_BASE + 0x18)
#define GICC_AIAR   (GICC_BASE + 0x20)
#define GICC_AEOIR  (GICC_BASE + 0x24)

#define GICRDIST(n) (0xa0000+(n)*0x20000)
